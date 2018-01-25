#!/usr/bin/python

import sys
import os
import cgi


# Show error as a page description.
sys.stderr = sys.stdout


# Get and parse a query string
query_string_key = 'QUERY_STRING'
if query_string_key in os.environ:
    query = cgi.parse_qs(os.environ[query_string_key])
else:
    query = {}


# Get and escape a MESSAGE
name_key = 'name'
if name_key in query:
    name = cgi.escape(query[name_key][0])
else:
    name = "default name"

text_key = 'text'
if text_key in query:
    text = cgi.escape(query[text_key][0])
else:
    text = "default text"


# Show MESSAGE
print """200 OK
Content-type: text/html


<html>
<head>
<title>%(name)s</title>
</head>
<body>
<h1>%(name)s</h1>
</body>
</html>
""" % {name_key:name}

