import http.server
import socketserver
import json
from pathlib import Path
from urllib.parse import urlparse

PORT = 8080
GUI_DIR = Path(__file__).resolve().parent
COMMAND_FILE = GUI_DIR / "command.txt"

class FileIPCBridge(http.server.SimpleHTTPRequestHandler):
    
    def do_POST(self):
        # Extract the action from the URL (e.g., '/step' becomes 'step')
        action = urlparse(self.path).path.strip('/')
        commands = {
            "run": "RUN\n",
            "pause": "PAUSE\n",
            "step": "STEP\n",
            "reset": "RESET\n",
            "load": "LOAD\n\n",
        }

        if action not in commands:
            self.send_response(404)
            self.send_header('Content-type', 'application/json')
            self.end_headers()
            self.wfile.write(b'{"status": "error", "message": "unknown command"}')
            return

        COMMAND_FILE.write_text(commands[action])

        # Tell the browser the command was routed
        self.send_response(200)
        self.send_header('Content-type', 'application/json')
        self.end_headers()
        self.wfile.write(b'{"status": "success"}')

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
