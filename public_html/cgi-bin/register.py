#!/usr/bin/env python
# -*- coding: utf-8 -*-

import cgi
import cgitb
import os
import sys
import codecs
import Cookie

sys.stdout = codecs.getwriter('utf_8')(sys.stdout)

cgitb.enable()

print ('Content-type: text/html; charset=UTF-8')
print ("\r\n\r\n")

#POSTデータ判定
if ( os.environ['REQUEST_METHOD'] != "POST" ):
    print (u"METHOD不正")
    sys.exit()

form = cgi.FieldStorage()
#パラメータチェック
if not ( form.has_key("name") and form.has_key("text") ):
    print (u"パラメータ不正")
    sys.exit()

#ファイル書込処理
file = None
try:
    file = open("/tmp/pelmanism/data.txt", "a")
except IOError, (errno, msg):
    print ('except: Cannot open file')
    print ('errno: [%d] msg: [%s]' % (errno, msg))

    print (os.popen('whoami').read())
else:
    try:
        file.write(form["name"].value + "," + form["text"].value + "\r\n")
        print (u"書けた")
    except IOError:
        print (u"書けなかった")
    finally:
        if(file):
            file.close()

print ('<script>location.href="http://10.0.0.10/~nijinoryu";</script>')
