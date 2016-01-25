#!/usr/bin/python2
#coding=utf-8
import HTMLParser
#import htmlentitydefs
import urllib2
import re
import sys

class AcademicParser(HTMLParser.HTMLParser):
    def __init__(self):
        HTMLParser.HTMLParser.__init__(self)
        self.text=''
        self.time=''
        self.link=''
        self.site=''
        self.items=[]
        self.content_flag=False
        self.parse_time_flag=False
        self.time_flag=False
        self.site_flag=False

    def handle_starttag(self,tag,attrs):
        if tag=='a' and attrs:
            for key,value in attrs:
                if key=='class' and 'ablue02' in value:
                    self.content_flag=True
                    self.parse_time_flag=True
                    self.site_flag=True
                elif key=='href':
                    self.link=value
        elif tag=='td':
            if  self.parse_time_flag==True:
                self.time_flag=True

    def handle_endtag(self,tag):
        if tag=='a':
            self.content_flag=False

    def handle_data(self,data):
        if self.content_flag==True:
            self.text+=data
        if self.time_flag==True:
            self.time=data
            self.items.append([self.text,self.time,self.site])
            self.text=self.time=self.site=''
            self.time_flag=self.parse_time_flag=False

    def handle_comment(self,data):
        if self.site_flag==True:
            self.site=data

    #def handle_entityref(self,name):
    #    if name in htmlentitydefs.entitydefs:
    #        self.text+=htmlentitydefs.entitydefs[name]
    #    else:
    #        self.text+=('&'+name+';')

    def get_items(self):
        return self.items

if len(sys.argv)==2 and sys.argv[1]=='-p':
    url='http://employ.scut.edu.cn:8880/zpzc/index.jsp'
elif len(sys.argv)==2 and sys.argv[1]=='-a':
    url='http://202.38.194.183:8880/zpzc/index.jsp'
else:
    print '-p:public network\n-a:academic network'

try:
    header={'User-Agent':'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.66 Safari/537.36','Refer':url}
    req=urllib2.Request(url=url,headers=header)
    f=urllib2.urlopen(req)
    academic_parser=AcademicParser()
    academic_parser.feed( f.read() )
except ( KeyboardInterrupt,NameError ):
    flag=False
    print 'Goodbye!'
except IOError:
    flag=False
    print 'Network N/A'

flag=True
try:
    if flag==True:
        items=academic_parser.get_items()
        for text,date,site in items:
            t=''.join(text.decode('gb2312').split())
            k=re.search('<.*>(.*)<.*>',site.decode('gb2312')).group(1)
            print t,'-'*(70-len(text)),date.decode('gb2312')
            print k.split()[0][:11].rjust(73)
except AttributeError:
    print t
except NameError:
    pass