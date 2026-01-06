#!/usr/bin/env python3

import cgi
import os
import sys
from urllib.parse import parse_qs

def main():
    # Read POST data
    content_length = int(os.environ.get('CONTENT_LENGTH', 0))
    post_data = sys.stdin.read(content_length)
    
    # Parse form data
    params = parse_qs(post_data)
    
    filename = params.get('filename', [''])[0]
    content = params.get('content', [''])[0]
    
    # Validate input
    if not filename or not content:
        print("Content-Type: text/html\r\n")
        print("<html><body><h1>Error: Missing filename or content</h1></body></html>")
        return
    
    # Sanitize filename (remove path traversal attempts)
    filename = os.path.basename(filename)
    
    # Define upload directory
    upload_dir = os.path.dirname(os.path.abspath(__file__)) + "/../uploads/"
    
    # Create uploads directory if it doesn't exist
    if not os.path.exists(upload_dir):
        os.makedirs(upload_dir)
    
    file_path = os.path.join(upload_dir, filename)
    
    try:
        # Create the file with the content
        with open(file_path, 'w') as f:
            f.write(content)
        
        # Return success response
        print("Content-Type: text/html\r\n")
        print("<html>")
        print("<head><title>File Created</title></head>")
        print("<body>")
        print(f"<h1>Success!</h1>")
        print(f"<p>File '{filename}' has been created successfully.</p>")
        print("<p><a href='../pages/uploads.html'>Go back</a></p>")
        print("</body>")
        print("</html>")
    except Exception as e:
        print("Content-Type: text/html\r\n")
        print("<html>")
        print("<head><title>Error</title></head>")
        print("<body>")
        print(f"<h1>Error creating file</h1>")
        print(f"<p>{str(e)}</p>")
        print("<p><a href='../pages/uploads.html'>Go back</a></p>")
        print("</body>")
        print("</html>")

if __name__ == "__main__":
    main()
