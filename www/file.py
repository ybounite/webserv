#!/usr/bin/env python3
"""
RFC 3875 (CGI 1.1) compliant CGI script
Displays environment variables and handles POST/GET requests
"""
import os
import sys
import cgi
import html
from urllib.parse import parse_qs

def get_request_body():
    """Read request body from stdin (RFC 3875)"""
    try:
        content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    except (ValueError, TypeError):
        content_length = 0
    
    if content_length > 0:
        return sys.stdin.read(content_length)
    return ""

def parse_form_data():
    """Parse POST form data and query string"""
    data = {}
    
    # Parse query string
    query_string = os.environ.get('QUERY_STRING', '')
    if query_string:
        data.update(parse_qs(query_string))
    
    # Parse POST body
    request_method = os.environ.get('REQUEST_METHOD', 'GET')
    if request_method == 'POST':
        content_type = os.environ.get('CONTENT_TYPE', '')
        if 'application/x-www-form-urlencoded' in content_type:
            body = get_request_body()
            if body:
                data.update(parse_qs(body))
    
    return data

def generate_html():
    """Generate RFC 3875 compliant response"""
    form_data = parse_form_data()
    
    html_content = """<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>CGI Environment Variables</title>
    <style>
        * { box-sizing: border-box; }
        body {
            font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, sans-serif;
            margin: 0;
            padding: 20px;
            background-color: #f5f5f5;
        }
        .container {
            max-width: 900px;
            margin: 0 auto;
            background: white;
            border-radius: 8px;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
            padding: 30px;
        }
        h1 {
            color: #333;
            border-bottom: 3px solid #007bff;
            padding-bottom: 10px;
            margin-top: 0;
        }
        h2 {
            color: #555;
            margin-top: 30px;
            border-bottom: 1px solid #ddd;
            padding-bottom: 8px;
        }
        .env-container {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(400px, 1fr));
            gap: 10px;
            margin: 15px 0;
        }
        .env-var {
            background-color: #f9f9f9;
            border-left: 4px solid #007bff;
            padding: 12px;
            border-radius: 4px;
            font-family: "Monaco", "Courier New", monospace;
            font-size: 13px;
            overflow-x: auto;
        }
        .key {
            color: #007bff;
            font-weight: bold;
        }
        .value {
            color: #333;
            word-break: break-all;
        }
        .form-section {
            background: #f0f7ff;
            border: 1px solid #bfd7f8;
            border-radius: 4px;
            padding: 15px;
            margin: 20px 0;
        }
        .form-group {
            margin-bottom: 15px;
        }
        label {
            display: block;
            margin-bottom: 5px;
            font-weight: bold;
            color: #333;
        }
        input, textarea {
            width: 100%;
            padding: 8px;
            border: 1px solid #ccc;
            border-radius: 4px;
            font-family: sans-serif;
        }
        textarea {
            resize: vertical;
            min-height: 100px;
        }
        button {
            background: #007bff;
            color: white;
            padding: 10px 20px;
            border: none;
            border-radius: 4px;
            cursor: pointer;
            font-size: 14px;
        }
        button:hover {
            background: #0056b3;
        }
        .data-display {
            background: #fff3cd;
            border: 1px solid #ffc107;
            border-radius: 4px;
            padding: 15px;
            margin: 15px 0;
        }
        .data-item {
            padding: 8px;
            background: white;
            margin: 5px 0;
            border-left: 3px solid #ffc107;
            padding-left: 10px;
        }
        footer {
            margin-top: 40px;
            text-align: center;
            color: #999;
            font-size: 12px;
            border-top: 1px solid #eee;
            padding-top: 20px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>RFC 3875 (CGI 1.1) Demo</h1>
        <p>This script demonstrates a standards-compliant CGI implementation.</p>
        
        <h2>Submit Data (GET or POST)</h2>
        <div class="form-section">
            <form method="POST" action="">
                <div class="form-group">
                    <label for="name">Name:</label>
                    <input type="text" id="name" name="name" placeholder="Enter your name">
                </div>
                <div class="form-group">
                    <label for="message">Message:</label>
                    <textarea id="message" name="message" placeholder="Enter a message"></textarea>
                </div>
                <button type="submit">Submit</button>
            </form>
        </div>
"""
    
    if form_data:
        html_content += """        <h2>Submitted Data</h2>
        <div class="data-display">
"""
        for key, values in sorted(form_data.items()):
            for value in values:
                html_content += f'            <div class="data-item"><strong>{html.escape(key)}:</strong> {html.escape(value)}</div>\n'
        html_content += "        </div>\n"
    
    html_content += """        <h2>Request Environment (RFC 3875 CGI Variables)</h2>
        <div class="env-container">
"""
    
    # RFC 3875 required CGI variables
    cgi_vars = [
        'CONTENT_LENGTH', 'CONTENT_TYPE', 'GATEWAY_INTERFACE', 
        'PATH_INFO', 'PATH_TRANSLATED', 'QUERY_STRING', 'REMOTE_ADDR',
        'REMOTE_HOST', 'REMOTE_PORT', 'REQUEST_METHOD', 'REQUEST_URI',
        'SCRIPT_FILENAME', 'SCRIPT_NAME', 'SERVER_NAME', 'SERVER_PORT',
        'SERVER_PROTOCOL', 'SERVER_SOFTWARE', 'HTTPS'
    ]
    
    for var in sorted(cgi_vars):
        if var in os.environ:
            value = os.environ[var]
            html_content += f'            <div class="env-var"><span class="key">{var}</span> = <span class="value">{html.escape(value)}</span></div>\n'
    
    html_content += """        </div>
        
        <h2>All Environment Variables</h2>
        <div class="env-container">
"""
    
    for key in sorted(os.environ.keys()):
        if key not in cgi_vars and not key.startswith('_'):
            value = os.environ[key]
            html_content += f'            <div class="env-var"><span class="key">{key}</span> = <span class="value">{html.escape(value)}</span></div>\n'
    
    html_content += """        </div>
        
        <footer>
            <p>RFC 3875 (CGI Version 1.1) Specification Compliant</p>
        </footer>
    </div>
</body>
</html>
"""
    return html_content

def main():
    """Main CGI entry point - RFC 3875 compliant"""
    try:
        # Generate response
        body_content = generate_html()
        body_bytes = body_content.encode('utf-8')
        content_length = len(body_bytes)
        
        # Send HTTP status line (required by RFC 3875)
        sys.stdout.write("HTTP/1.1 200 OK\r\n")
        
        # Send headers (required by RFC 3875)
        sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n")
        sys.stdout.write(f"Content-Length: {content_length}\r\n")
        sys.stdout.write("Cache-Control: no-cache, no-store, must-revalidate\r\n")
        sys.stdout.write("\r\n")
        
        # Send body
        sys.stdout.write(body_content)
        
        sys.stdout.flush()
        
    except Exception as e:
        # Error handling - send 500 error per RFC 3875
        error_msg = f"CGI Error: {str(e)}"
        error_html = f"""HTTP/1.1 500 Internal Server Error\r
Content-Type: text/plain\r
Content-Length: {len(error_msg)}\r
\r
{error_msg}"""
        sys.stdout.write(error_html)
        sys.stdout.flush()
        sys.exit(1)

if __name__ == '__main__':
    main()
