#!/usr/bin/python2
#encoding=utf-8
import urllib
import json
import sys
import wx

"An easy youdao-dict"

def trans(str):
    "Translation using youdao's lib"
    while True:
        ret_val=''
        try:
            url='http://fanyi.youdao.com/openapi.do?keyfrom=Cheukyin&key=1618181256&type=data&doctype=json&version=1.1&q='+str
            #header={'User-Agent':'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.66 Safari/537.36','Accept-Language':'en,zh-CN;q=0.8,zh;q=0.6'}
            #req=urllib2.Request(url=url,headers=header)
            #f=urllib2.urlopen(req).read()
            f=urllib.urlopen(url).read()
            json_r=json.loads(f)
        except ValueError:
            break
        except urllib.URLError:
            return 'Network N/A'

        try:
            t=json_r.get('basic')
            u=t.get('phonetic')
            v=t.get('explains')
            if u==None:
                ret_val+=u'未找到发音\n'
            else:
                ret_val+=u'发音：'+'[ '+u+' ]\n'
            ret_val+=u'解释：\n'
            for each in v:
                ret_val+=each+'\n'
        except AttributeError:
            ret_val+=str+' not in the database\n'
        return ret_val

def command_cli():
    "Command Line"
    while True:
        try:
            str=raw_input("输入:")
        except ( EOFError,KeyboardInterrupt ):
            print '\nGoodbye!'
            break
        print trans(str)

def gui():
    "GUI-Wxpython"
    app=wx.App()
    win=wx.Frame(None,title=u'简易有道词典',size=(400,150))
    bkg=wx.Panel(win)

    def query(str):
        contents.SetValue(trans(word.GetValue().encode('utf-8')))

    querybutton=wx.Button(bkg,label=u'查询')
    querybutton.Bind(wx.EVT_BUTTON,query)

    word=wx.TextCtrl(bkg)
    contents=wx.TextCtrl(bkg,style=wx.TE_MULTILINE|wx.HSCROLL)

    hbox=wx.BoxSizer()
    hbox.Add(word,proportion=3,flag=wx.EXPAND)
    hbox.Add(querybutton,proportion=0,flag=wx.LEFT,border=5)

    vbox=wx.BoxSizer(wx.VERTICAL)
    vbox.Add(hbox,proportion=0,flag=wx.EXPAND|wx.ALL,border=5)
    vbox.Add(contents,proportion=1,flag=wx.EXPAND|wx.LEFT|wx.BOTTOM|wx.RIGHT,border=5)

    bkg.SetSizer(vbox)
    win.Show()
    app.MainLoop()

def dis_help():
    "Display help"
    print '-c:commandline\n-g:GUI\n-h:help'

if __name__=='__main__':
    if len(sys.argv)==1 or sys.argv[1]=='-c':
        command_cli()
    elif sys.argv[1]=='-g':
        gui()
    else:
        dis_help