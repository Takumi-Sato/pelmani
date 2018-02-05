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
    fileName = "/home/xiao/pelmani/txt_data"
    files = os.listdir(fileName)
    cnt = 1
    for f in files:
        cnt = cnt + 1
    fileName = fileName + "/" + str(cnt) + ".txt"

    file = open(fileName, "w")

except IOError, (errno, msg):
    print ('except: Cannot open file')
    print ('errno: [%d] msg: [%s]' % (errno, msg))

    print (os.popen('whoami').read())
else:
    try:
        file.write(form["name"].value + ":" + form["text"].value + "\r\n")
        '''
        ーーーーー　ココから元のコード　－－－－－
        print (u"<html lang=ja><head><title>送信完了</title></head><body><h2>送信完了しました！</h2><div>名前 : ")
        print form["name"].value.decode('utf-8')
        print (u"</div><div>秘密 : ")
        himitsu = form["text"].value.decode('utf-8')
        print (himitsu)
        print (u"</div></body></html>")
        ーーーーー　ココまで元のコード　ーーーーー
        '''

        print (u"<!DOCTYPE html><html lang=\"en\"><head><meta charset=\"UTF-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\"><meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\"><link rel=\"stylesheet\" href=\"http://172.16.0.1/css/comp.css\"><title>Pelmanion</title></head><body><header class=\"header\"></header><div class=\"contents\"><div class=\"title\"><img src=\"http://172.16.0.1/pic/logo.png\" alt=\"logo\"></div><h2>送信しました.</h2><div class=\"message\"><p class=\"massage_name\"><span class=\"under\"><span class=\"under_b\">")
        print form["name"].value.decode('utf-8')
        print (u"さん</span>のカミングアウトです。</span></br></p><p class=\"massage_text\"><span class=\"under\">")
        himitsu = form["text"].value.decode('utf-8')
        print (himitsu)
        print (u"</span></p></div><button type=\"button\"onclick=\"location.href='http://172.16.0.1/index.html'\" class=\"back_button\">トップに戻る</button></div><!--contents--><footer class=\"footer\">CampusOJT-SW   9th</footer></body></html>")
                                                
    except IOError:
        print (u"書き込めませんでした.")
    finally:
        if(file):
            file.close()

#print ('<script>location.href="http://172.16.0.1";</script>')
