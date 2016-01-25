#!/usr/bin/python2
#encoding=utf-8
from bs4 import BeautifulSoup
import cookielib
import urllib2
import urllib
import getpass
import re

def open_url(url):
    "open the url"

    header={'User-Agent':'Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.66 Safari/537.36'}
    req=urllib2.Request(url=url,headers=header)
    return urllib2.urlopen(req).read()


def get_cookie(login_page,username,user_name,password,pwd):
    "get cookie"

    #get a CookieJar object
    cj = cookielib.CookieJar()
    #build a opener
    opener=urllib2.build_opener(urllib2.HTTPCookieProcessor(cj))
    #disguise as a web-browser
    opener.addheaders = [('User-agent','Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/30.0.1599.66 Safari/537.36')]
    #generate the Post data with username&password
    data = urllib.urlencode({username:user_name,password:pwd})

    #visit the login page in Post way,CookieJar will save the cookie
    opener.open(login_page,data)

    return opener


def partimejob_info(info_page):
    "get the partimejob_info outside the school"

    info=open_url(info_page)
    soup=BeautifulSoup(info)

    #find all a tags
    a_tag=[]
    for each in soup.find_all('a'):
        try:
            if u'parttime_' in each.get('href'):
                a_tag.append(each)
        except TypeError:
            pass

    continue_opt='y'
    while continue_opt=='y' or continue_opt=='Y':
        i=0
        print

        #filter and print the a tags desired
        for each in a_tag:
            t=each['title']
            date=each.parent.next_sibling.next_sibling.string
            if len(t)>40:
                t=t[:39]+'..'
            print repr(i)+'.',t,'-'*(50-len(t)),date
            i+=1

        #read the details of the chosen information
        option=raw_input("Your choice(q to abandon):")
        url=info_page + a_tag[int(option)]['href']
        info=open_url(url)
        soup=BeautifulSoup(info)
        for each in soup.find_all('td')[3:-3]:
            print each.get_text()

        #Choose to sign up the job or not
        signup_opt=raw_input('\nSign up or not(Y/N)?')
        if signup_opt!='N' and signup_opt!='n':
            signup_ID=re.search("return Sign_Up\((\d+)\)",info).group(1)
            url=info_page+'sign_up.jsp?pID='+signup_ID
            info=open_url(url)
            if u'请在发布日期和截止日期之间报名' in info.decode('gb2312'):
                print "请在发布日期和截止日期之间报名"

        #Continue or not
        continue_opt=raw_input('\nContinue(Y/N)?')
    print


def teacher_login(login_page,user_name,pwd):
    "login the page to get self-information"

    global log_stat

    print
    username="tName"
    password="tPwd"
    opener=get_cookie(login_page,username,user_name,password,pwd)

    url='http://www.scutqgzx.com/user_admin/'
    url_useradmin_status=url+'left.jsp'

    try:
        #open the url with cookie
        op=opener.open(url_useradmin_status)
        info=op.read()
        soup=BeautifulSoup(info)
        info=soup.get_text()
        print re.search(u'你好\S+',info).group()
        print re.search(u'家教状态\S+',info).group()
        print '1.'+re.search(u'收到短信\S+',info).group()
        print '2.'+re.search(u'已发短信\S+',info).group()

        option=raw_input("Your choice(q to abandon):")
        if option=='1':
            url_msg=url+'msg_r.jsp'
        if option=='2':
            url_msg=url+'msg_s.jsp'
        info=opener.open(url_msg).read()
        soup=BeautifulSoup(info)
        print soup.table.get_text()
    except AttributeError:
        log_stat=False
        print '用户名或密码错误！'


if __name__=='__main__':

    log_stat=False
    info_page="http://www.scutqgzx.com/"
    login_page='http://www.scutqgzx.com/login_chk.do'

    while True:
        print u"1.校外兼职"
        if log_stat==False:
            print u"2.教员登录"
        else:
            print u"2.教员信息"
            print u"3.退出登录"
        try:
            option=raw_input("Your choice:")
            if option=='1':
                partimejob_info(info_page)
            elif option=='2':
                if log_stat==False:
                    user_name=raw_input("User_Name:")
                    pwd=getpass.getpass("PassWord:")
                    log_stat=True
                teacher_login(login_page,user_name,pwd)
            elif option=='3':
                log_stat=False
                print
            else:
                break
        except ( KeyboardInterrupt,EOFError ):
            break
        except (ValueError,UnboundLocalError):
            print
    print '\nGoodbye!'