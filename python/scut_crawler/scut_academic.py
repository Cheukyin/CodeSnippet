#!/usr/bin/python2
#encoding='utf-8'
import HTMLParser
import urllib2

class AcademicParser(HTMLParser.HTMLParser):
    def __init__(self):
        HTMLParser.HTMLParser.__init__(self)
        self.text=''
        self.time=''
        self.link=''
        self.items=[]
        self.content_flag=False
        self.parse_time_flag=False
        self.time_flag=False

    def handle_starttag(self,tag,attrs):
        if tag=='a' and attrs:
            for key,value in attrs:
                if key=='id' and 'exerciseContent' in value:
                    self.content_flag=True
                elif key=='href':
                    self.link=value
        elif tag=='span':
            for  key,value in attrs:
                if key=='id' and 'createTime' in value:
                    self.parse_time_flag=True
        elif tag=='font' and self.parse_time_flag==True:
            self.time_flag=True

    def handle_data(self,data):
        if self.content_flag==True:
            self.text=data
            self.content_flag=False
        if self.time_flag==True:
            self.time=data
            self.items.append([self.text,self.time])
            self.time_flag=self.parse_time_flag=False

    def get_items(self):
        return self.items

try:
    url='http://202.38.193.235/jiaowuchu/%E9%A6%96%E9%A1%B5/%E6%95%99%E5%8A%A1%E9%80%9A%E7%9F%A51/more.aspx'
    header={'User-Agent':'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.66 Safari/537.36','Refer':url}
    req=urllib2.Request(url=url,headers=header)
    f=urllib2.urlopen(req)
    academic_parser=AcademicParser()
    academic_parser.feed( f.read() )
    items=academic_parser.get_items()
    for text,date in items:
        t=text.decode('gb2312')
        if len(t)>40:
            t=t[:39]+'..'
        print t,'-'*(76-len(text)),date.split()[0]
except KeyboardInterrupt:
    flag=False
    print 'Goodbye!'
except IOError:
    flag=False
    print 'Network N/A'