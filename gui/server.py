import http.server
import socketserver
import json
from pathlib import Path
from urllib.parse import urlparse

PORT = 8080
GUI_DIR = Path(__file__).resolve().parent
PROGRAMS_DIR = GUI_DIR.parent / "programs"
COMMAND_FILE = GUI_DIR / "command.txt"

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
        action = urlparse(self.path).path.strip('/')
        commands = {
            "run": "RUN\n",
            "pause": "PAUSE\n",
            "step": "STEP\n",
            "reset": "RESET\n",
        }

        if action == "load":
            content_length = int(self.headers.get("Content-Length", 0))
            payload = {}

            if content_length > 0:
                try:
                    payload = json.loads(self.rfile.read(content_length))
                except json.JSONDecodeError:
                    self.send_json(400, {"status": "error", "message": "invalid JSON"})
                    return

            filename = payload.get("filename", "")
            program_path = PROGRAMS_DIR / filename

            if (
                not filename
                or Path(filename).name != filename
                or not program_path.is_file()
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
