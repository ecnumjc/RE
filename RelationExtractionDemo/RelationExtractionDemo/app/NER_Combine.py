#coding:utf-8
import os
import re

def data_combine(strfin,strfout): 
    fin=open(strfin)
    fout=open(strfout,'w')

    nerstr = ""
    nernum=0
     
    per_str=''
    perb_str=''
    fm_str=''
    com_str=''
    time_str=''
    loc_str='' 
    type_str=''       
    
    strper=''
    strperb=''
    strfm=''
    strcom=''
    strtime=''
    strloc='' 
    strtype=''  

    pertag=0
    perbtag=0
    fmtag=0
    comtag=0
    timetag =0
    loctag=0
    typetag=0
    line_list = []
    for line in fin:
        if len(line.strip().replace('\n','')):
            line_list.append(line)
    for j in range(len(line_list)):
        li = line_list[j].strip().replace('\n','').split('\t')
        PER = "artist"
        perlabel ="<Na>"+PER+"</Na> "
        PERB = "artistb"
        perblabel ="<Nab>"+PER+"</Nab> "
        FM = 'fm'
        fmlabel ='<Nf>'+FM+'</Nf> '
        COM='com'
        comlabel = '<Nc>'+COM+'</Nc> '
        TIME = 'time'
        timelabel='<Nd>'+TIME+'</Nd> '
        LOC = 'loc'
        loclabel ='<Nl>'+LOC+'</Nl> '
        TY='type'
        typelabel='<Nt>'+TY+'</Nt> '
      
        if li[2]=='Na':
            strper = strper+''.join(li[0])
            strinfo = re.compile(PER)
            per_str = strinfo.sub(strper,perlabel)
            pertag=1
        if li[2]!='Na' and pertag ==1:
            nerstr =nerstr+per_str
            PER = "artist"
            perlabel ="<Na>"+PER+"</Na> "
            pertag =0
                
        if li[2]=='Nab':
            strperb = strperb+''.join(li[0])
            strinfo = re.compile(PERB)
            perb_str = strinfo.sub(strper,perblabel)
            perbtag=1
        if li[2]!='Nab' and perbtag ==1:
            nerstr =nerstr+perb_str
            PERB = "artistb"
            perblabel ="<Nab>"+PERB+"</Nab> "
            perbtag =0

        if li[2] == 'Nf':
            strfm =strfm+ ''.join(li[0])
            strinfo = re.compile(FM)
            fm_str = strinfo.sub(strfm,fmlabel)
            fmtag=1
        if li[2] != 'Nf' and fmtag==1:
            nerstr = nerstr+fm_str
            FM = 'fm'
            fmlabel ='<FM>'+FM+'</FM> '
            fmtag =0

        if li[2] == 'Nc':
            strcom =strcom+ ''.join(li[0])
            strinfo = re.compile(COM)
            com_str = strinfo.sub(strcom,comlabel)
            comtag=1
        if li[2] != 'Nc' and comtag==1:
            nerstr = nerstr+com_str
            COM = 'com'
            comlabel ='<Nc>'+COM+'</Nc> '
            comtag =0

        if li[2]=='Nd':
            strtime = strtime+''.join(li[0])
            strinfo = re.compile(TIME)
            time_str = strinfo.sub(strtime,timelabel)
            timetag=1    
        if li[2]!='Nd' and timetag==1:
            nerstr = nerstr +time_str
            TIME ='time'
            timelabel='<Nd>'+TIME+'</Nd> '
            timetag=0

        if li[2] =='Nl':
            strloc = strloc+''.join(li[0])
            strinfo = re.compile(LOC)
            loc_str = strinfo.sub(strloc,loclabel)
            loctag=1
        if li[2] !='Nl' and loctag==1:
            nerstr =nerstr+loc_str
            LOC = 'loc'
            loclabel ='<Nl>'+LOC+'</Nl> '
            loctag=0

        if li[2] =='Nt':
            strtype = strtype+''.join(li[0])
            strinfo = re.compile(TY)
            type_str = strinfo.sub(strtype,typelabel)
            typetag=1
        if li[2] !='Nt' and typetag==1:
            nerstr =nerstr+type_str
            TY = 'loc'
            typelabel ='<Nt>'+TY+'</Nt> '
            typetag=0

        if li[2]=='N':
            per_str=''
            perb_str=''
            fm_str=''
            com_str=''
            time_str=''
            loc_str='' 
            type_str=''       
            
            strper=''
            strperb=''
            strfm=''
            strcom=''
            strtime=''
            strloc='' 
            strtype=''  
            
            pertag=0
            perbtag=0
            fmtag=0
            comtag=0
            timetag =0
            loctag=0
            typetag=0
            nerstr = nerstr + li[0] + '/' + li[1]+' '
            #nerstr = nerstr + li[0]+' '

    napattern=re.compile('<Na>')
    nabpattern=re.compile('<Nab>')
    nfpattern=re.compile('<Nf>')
    ncpattern=re.compile('<Nc>')
    ndpattern=re.compile('<Nd>')
    nlpattern=re.compile('<Nl>')
    ntpattern=re.compile('<Nt>')
    if len(napattern.findall(nerstr)):
        nernum+=1
    if len(nabpattern.findall(nerstr)):
        nernum+=1
    if len(nfpattern.findall(nerstr)):
        nernum+=1
    if len(ncpattern.findall(nerstr)):
        nernum+=1
    if len(ndpattern.findall(nerstr)):
        nernum+=1
    if len(nlpattern.findall(nerstr)):
        nernum+=1
    if len(ntpattern.findall(nerstr)):
        nernum+=1


    if nernum>=2:
        fout.write(nerstr+'\n') 
        fout.close()
    fin.close()

