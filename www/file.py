#!/usr/bin/env python3
import sys
try:
    print("Hello, World!")
    sys.stdout.flush()
except BrokenPipeError:
    # client closed the pipe
    sys.exit(0)