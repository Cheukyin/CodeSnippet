#!/usr/bin/python2
import wx
import sys

class Frame(wx.Frame):
    def __init__(self,image,parent=None,id=-1,pos=wx.DefaultPosition,title='Hello,Wxpython!'):
        temp=image.ConvertToBitmap()
        size=temp.GetWidth(),temp.GetHeight()
        wx.Frame.__init__(self,parent,id,title,pos,size)
        self.bmp=wx.StaticBitmap(parent=self,bitmap=temp)

class App(wx.App):
    def __init__(self,redirect=True,filename=None):
        print "App__init__"
        wx.App.__init__(self,redirect,filename)

    def OnInit(self):
        image=wx.Image('0.jpg',wx.BITMAP_TYPE_JPEG)
        self.frame=Frame(image)
        self.frame.Show()
        self.SetTopWindow(self.frame)
        return True

if __name__=='__main__':
    app=App()
    app.MainLoop()