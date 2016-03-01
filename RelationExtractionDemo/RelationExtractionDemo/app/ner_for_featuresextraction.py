#coding:utf-8
import re
 
def ChangeLabel(line):
    strline = line.replace('<Na>','')
    strline = strline.replace('<Nab>','')
    strline = strline.replace('<Nf>','')
    strline = strline.replace('<Nc>','')
    strline = strline.replace('<Nd>','')
    strline = strline.replace('<Nl>','')
    strline = strline.replace('<Nt>','') 
     
    #strline = strline.replace('</Na>','')
    #strline = strline.replace('</Nab>','')
    #strline = strline.replace('</Nf>','')
    #strline = strline.replace('</Nc>','')
    #strline = strline.replace('</Nd>','')
    #strline = strline.replace('</Nl>','')
    #strline = strline.replace('</Nt>','')
         
    strline = strline.replace('</Na>','/Na')
    strline = strline.replace('</Nab>','/Nab')
    strline = strline.replace('</Nf>','/Nf')
    strline = strline.replace('</Nc>','/Nc')
    strline = strline.replace('</Nd>','/Nd')
    strline = strline.replace('</Nl>','/Nl')
    strline = strline.replace('</Nt>','/Nt')
    return strline


def meiju(line):
    li = []
    li_Na = []
    li_Nab = []
    li_Nf = []
    li_Nc = []
    li_Nd = []
    li_Nl = []
    li_Nt = []

    p_Na = re.compile(r'<Na>.*?</Na>')
    p_Nab = re.compile(r'<Nab>.*?</Nab>')
    p_Nf = re.compile(r'<Nf>.*?</Nf>')
    p_Nc = re.compile(r'<Nc>.*?</Nc>')
    p_Nd = re.compile(r'<Nd>.*?</Nd>')
    p_Nl = re.compile(r'<Nl>.*?</Nl>')
    p_Nt = re.compile(r'<Nt>.*?</Nt>')

    label_Na = '2016_labelNer_mjc/Na'
    label_Nab = '2016_labelNer_mjc/N_ab'
    label_Nf = '2016_labelNer_mjc/Nf'
    label_Nc = '2016_labelNer_mjc/Nc'
    label_Nd = '2016_labelNer_mjc/Nd'
    label_Nl = '2016_labelNer_mjc/Nl'
    label_Nt = '2016_labelNer_mjc/Nt'

    for m in p_Na.finditer(line):
        li_Na.append(m.group())
    for m in p_Nab.finditer(line):
        li_Nab.append(m.group())
        print m.group()
    for m in p_Nf.finditer(line):
        li_Nf.append(m.group())
    for m in p_Nc.finditer(line):
        li_Nc.append(m.group())
    for m in p_Nd.finditer(line):
        li_Nd.append(m.group())
    for m in p_Nl.finditer(line):
        li_Nl.append(m.group())
    for m in p_Nt.finditer(line):
        li_Nt.append(m.group())

    #Na-Nf
    for i in range(len(li_Na)):
        for j in range(len(li_Nf)):
            nanf = line
            nanf = re.sub(li_Na[i],label_Na,nanf)
            nanf = re.sub(li_Nf[j],label_Nf,nanf)
            strnanf = ChangeLabel(nanf)
            strnanf = re.sub(label_Na,li_Na[i],strnanf)
            strnanf = re.sub(label_Nf,li_Nf[j],strnanf)
            li.append(strnanf)
    #Nf-Nc
    for i in range(len(li_Nf)):
        for j in range(len(li_Nc)):
            nfnc = line
            nfnc = re.sub(li_Nf[i],label_Nf,nfnc)
            nfnc = re.sub(li_Nc[j],label_Nc,nfnc)
            strnfnc = ChangeLabel(nfnc)
            strnfnc = re.sub(label_Nf,li_Nf[i],strnfnc)
            strnfnc = re.sub(label_Nc,li_Nc[j],strnfnc)
            li.append(strnfnc)
    #Nf-Nd
    for i in range(len(li_Nf)):
        for j in range(len(li_Nd)):
            nfnd = line
            nfnd = re.sub(li_Nf[i],label_Nf,nfnd)
            nfnd = re.sub(li_Nd[j],label_Nd,nfnd)
            strnfnd = ChangeLabel(nfnd)
            strnfnd = re.sub(label_Nf,li_Nf[i],strnfnd)
            strnfnd = re.sub(label_Nd,li_Nd[j],strnfnd)
            li.append(strnfnd)
    #Nf-Nl
    for i in range(len(li_Nf)):
        for j in range(len(li_Nl)):
            nfnl = line
            nfnl = re.sub(li_Nf[i],label_Nf,nfnl)
            nfnl = re.sub(li_Nl[j],label_Nl,nfnl)
            strnfnl = ChangeLabel(nfnl)
            strnfnl = re.sub(label_Nf,li_Nf[i],strnfnl)
            strnfnl = re.sub(label_Nl,li_Nl[j],strnfnl)
            li.append(strnfnl)
    #Nf-Nt
    for i in range(len(li_Nf)):
        for j in range(len(li_Nt)):
            nfnt = line
            nfnt = re.sub(li_Nf[i],label_Nf,nfnt)
            nfnt = re.sub(li_Nt[j],label_Nt,nfnt)
            strnfnt = ChangeLabel(nfnt)
            strnfnt = re.sub(label_Nf,li_Nf[i],strnfnt)
            strnfnt = re.sub(label_Nt,li_Nt[j],strnfnt)
            li.append(strnfnt)
    #Na-Nab
    for i in range(len(li_Na)):
        for j in range(len(li_Nab)):
            nanab = line
            print li_Na[i],li_Nab[j]
            nanab = re.sub(li_Na[i],label_Na,nanab)
            nanab = re.sub(li_Nab[j],label_Nab,nanab)
            strnanab = ChangeLabel(nanab)
            strnanab = re.sub(label_Na,li_Na[i],strnanab)
            strnanab = re.sub(label_Nab,li_Nab[j],strnanab)
            li.append(strnanab)
    return li


def moresents(strfin,strfout):
    fout = open(strfout,'w')
    with open(strfin) as fin:
        for line in fin:
            li=[]
            li=meiju(line)
            #li=meiju(line.replace(' ',''))
            for i in range(len(li)):
                fout.write(li[i])
        fin.close()
    fout.close()




#if __name__ == "__main__":
#    #moresents('../data/1.txt','../data/2.txt')
#    line = "·Ö±ğ/d ÊÇ/v £º/x <Na>²¼Âå¿Ë</Na> ¡¢/x <Na>ÖìÀò</Na> ¡¢/x <Na>Ã·ÀöÉ¯</Na> -/x <Na>Âó¿¨Îı</Na> (/x <Nab>artist</Nab> " "/x <Nab>artist</Nab> )/x ºÍ/c <Na>Õ²Äİ¸¥</Na> -/x <Na>ÀÍÂ×Ë¹</Na> (/x <Nab>artist</Nab> " "/x <Nab>artist</Nab> )/x "
#    L= meiju(line)
#    for i in range(len(L)):
#        print L[i]
