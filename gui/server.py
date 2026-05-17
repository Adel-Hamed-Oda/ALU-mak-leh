import http.server
import socketserver
import json
from pathlib import Path
from urllib.parse import urlparse

PORT = 8080
GUI_DIR = Path(__file__).resolve().parent
PROGRAMS_DIR = GUI_DIR.parent / "programs"
COMMAND_FILE = GUI_DIR / "command.txt"

def normalize_program_filename(filename):
    filename = filename.strip()

    if not filename:
        return None

    if not filename.endswith(".txt"):
        filename = f"{filename}.txt"

    if Path(filename).name != filename:
        return None

    return filename

class FileIPCBridge(http.server.SimpleHTTPRequestHandler):
    def end_headers(self):
        self.send_header("Cache-Control", "no-store")
        super().end_headers()

    def send_json(self, status, payload):
        body = json.dumps(payload).encode("utf-8")
        self.send_response(status)
        self.send_header("Content-type", "application/json")
        self.send_header("Content-Length", str(len(body)))
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        if urlparse(self.path).path == "/api/programs":
            PROGRAMS_DIR.mkdir(exist_ok=True)
            programs = sorted(
                path.name
                for path in PROGRAMS_DIR.iterdir()
                if path.is_file()
            )
            self.send_json(200, programs)
            return

        super().do_GET()

    def do_POST(self):
        # Extract the action from the URL (e.g., '/step' becomes 'step')
        path = urlparse(self.path).path
        action = path.strip('/')
        commands = {
            "run": "RUN\n",
            "pause": "PAUSE\n",
            "step": "STEP\n",
            "reset": "RESET\n",
        }

        if path == "/api/programs":
            content_length = int(self.headers.get("Content-Length", 0))

            if content_length <= 0:
                self.send_json(400, {"status": "error", "message": "missing program"})
                return

            try:
                payload = json.loads(self.rfile.read(content_length))
            except json.JSONDecodeError:
                self.send_json(400, {"status": "error", "message": "invalid JSON"})
                return

            filename = normalize_program_filename(payload.get("filename", ""))
            content = payload.get("content", "")

            if filename is None:
                self.send_json(400, {"status": "error", "message": "invalid filename"})
                return

            if not isinstance(content, str) or not content.strip():
                self.send_json(400, {"status": "error", "message": "empty program"})
                return

            PROGRAMS_DIR.mkdir(exist_ok=True)
            program_path = PROGRAMS_DIR / filename

            if program_path.exists():
                self.send_json(409, {"status": "error", "message": "program already exists"})
                return

            program_path.write_text(content.rstrip() + "\n", encoding="utf-8")
            self.send_json(201, {"status": "success", "filename": filename})
            return

        if action == "load":
            content_length = int(self.headers.get("Content-Length", 0))
            payload = {}

            if content_length > 0:
                try:
                    payload = json.loads(self.rfile.read(content_length))
                except json.JSONDecodeError:
                    self.send_json(400, {"status": "error", "message": "invalid JSON"})
                    return

            filename = normalize_program_filename(payload.get("filename", ""))

            if (
                not filename
                or not (PROGRAMS_DIR / filename).is_file()
            ):
                self.send_json(400, {"status": "error", "message": "invalid program"})
                return

            COMMAND_FILE.write_text(f"LOAD\n{filename}\n")
            self.send_json(200, {"status": "success"})
            return

        if action not in commands:
            self.send_json(404, {"status": "error", "message": "unknown command"})
            return

        COMMAND_FILE.write_text(commands[action])

        # Tell the browser the command was routed
        self.send_json(200, {"status": "success"})

# Serve static files correctly from the /gui folder
class CustomHandler(FileIPCBridge):
    def __init__(self, *args, **kwargs):
        super().__init__(*args, directory=str(GUI_DIR), **kwargs)

CustomHandler.extensions_map.update({
    ".js": "application/javascript",
    ".json": "application/json",
})

if __name__ == '__main__':
    print(f"API Bridge Online! Open http://localhost:{PORT}")
    with socketserver.ThreadingTCPServer(("", PORT), CustomHandler) as httpd:
        try:
            httpd.serve_forever()
        except KeyboardInterrupt:
            print("\nShutting down server...")
