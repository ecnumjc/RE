"""
Definition of views.
"""

from django.shortcuts import render
from django.http import HttpRequest
from django.template import RequestContext
from datetime import datetime

from django.http import HttpResponse         
from app.models import Wikiinfotable
import os
import json
import NER_Combine as ner_hb
import ner_for_featuresextraction as ner_meiju
import jieba
import jieba.posseg as pseg
jieba.initialize() 
from snownlp import SnowNLP




entityList=[]
relationList = []





def sentence_seg(sent):
    sent_seg = []
    sent_seg_temp = SnowNLP(sent)
    for i in range(len(sent_seg_temp.sentences)):
        try:
            sent_seg.append(sent_seg_temp.sentences[i])
        except:
            print 'error!\n'
    return sent_seg


def CRF_ner_test(modelname,testfilename,resultname):
    cmd = "crf_test -m "+modelname+" "+testfilename+" > "+resultname
    os.system(cmd)


def FilmNer(sent):
    result = pseg.cut(sent)
    fout = open("testtemp.txt",'w')
    for w in result:
        #print w.word,w.flag
        fout.write(w.word.encode('gbk')+'\t'+w.flag.encode('gbk')+'\n')
    fout.close()
    CRF_ner_test("ner_model","testtemp.txt","ner_result")     



def maxent(strfin,strfout):
    cmd='java -jar maxent_test_fat.jar '+strfin+' '+strfout
    os.system(cmd)

def CRF_relation(strfin):
    cmd_crf_relation='crf_test -m model_re '+strfin+' > result_relation'
    os.system(cmd_crf_relation)  
    with open("result_relation") as fin:
        li_relation =[]
        for line in fin:
            li = line.replace('\n','').split()
            if len(li):
                dict_relation ={}
                print li[0],li[1],li[3],li[4],li[-1]
                dict_relation['entity1type'] = li[0]
                dict_relation['entity2type'] = li[1]
                dict_relation['entity1'] = li[3]
                dict_relation['entity2'] = li[4]
                dict_relation['relation'] = li[-1]

                if li[0]=='Na':
                    dict1 ={}
                    dict1['category'] = 0
                    dict1['name'] = li[3].decode('gbk')
                    dict1['value'] = 5
                    entityList.append(dict1)

                if li[0]=='Nab':
                    dict1 ={}
                    dict1['category'] = 1
                    dict1['name'] = li[3].decode('gbk')
                    dict1['value'] = 7
                    entityList.append(dict1)
                
                if li[0]=='Nf':
                    dict1 ={}
                    dict1['category'] = 2
                    dict1['name'] = li[3].decode('gbk')
                    dict1['value'] = 6
                    entityList.append(dict1)
                                    
                if li[0]=='Nc':
                    dict1 ={}
                    dict1['category'] = 3
                    dict1['name'] = li[3].decode('gbk')
                    dict1['value'] = 10
                    entityList.append(dict1)
                                    
                if li[0]=='Nd':
                    dict1 ={}
                    dict1['category'] = 4
                    dict1['name'] = li[3].decode('gbk')
                    dict1['value'] = 4
                    entityList.append(dict1)
                                    
                if li[0]=='Nl':
                    dict1 ={}
                    dict1['category'] = 5
                    dict1['name'] = li[3].decode('gbk')
                    dict1['value'] = 6
                    entityList.append(dict1)
                                    
                if li[0]=='Nt':
                    dict1 ={}
                    dict1['category'] = 6
                    dict1['name'] = li[3].decode('gbk')
                    dict1['value'] = 4
                    entityList.append(dict1)
                                    
                if li[1]=='Na':
                    dict2 = {}
                    dict2['category'] = 0
                    dict2['name'] = li[4].decode('gbk')
                    dict2['value'] = 5
                    entityList.append(dict2)

                if li[1]=='Nab':
                    dict2 = {}
                    dict2['category'] = 1
                    dict2['name'] = li[4].decode('gbk')
                    dict2['value'] = 7
                    entityList.append(dict2)
                                    
                if li[1]=='Nf':
                    dict2 = {}
                    dict2['category'] = 2
                    dict2['name'] = li[4].decode('gbk')
                    dict2['value'] = 6
                    entityList.append(dict2)
                                    
                if li[1]=='Nc':
                    dict2 = {}
                    dict2['category'] = 3
                    dict2['name'] = li[4].decode('gbk')
                    dict2['value'] = 10
                    entityList.append(dict2)
                                    
                if li[1]=='Nd':
                    dict2 = {}
                    dict2['category'] = 4
                    dict2['name'] = li[4].decode('gbk')
                    dict2['value'] = 4
                    entityList.append(dict2)
                                    
                if li[1]=='Nl':
                    dict2 = {}
                    dict2['category'] = 5
                    dict2['name'] = li[4].decode('gbk')
                    dict2['value'] = 6
                    entityList.append(dict2)
                                    
                if li[1]=='Nt':
                    dict2 = {}
                    dict2['category'] = 6
                    dict2['name'] = li[4].decode('gbk')
                    dict2['value'] = 4
                    entityList.append(dict2)


                d1={}
                d1['source'] = li[3].decode('gbk')
                d1['target'] = li[4].decode('gbk')              
                if li[-1]=='1':
                    d1['weight'] = 3
                    d1['name'] = '导演——影视名'
                if li[-1]=='2':
                    d1['weight'] = 4
                    d1['name'] = '演员——影视名'
                if li[-1]=='3':
                    d1['weight'] = 4
                    d1['name'] = '编剧——影视名'
                if li[-1]=='4':
                    d1['weight'] = 4
                    d1['name'] = '监制——影视名'
                if li[-1]=='5':
                    d1['weight'] = 3
                    d1['name'] = '影视名——出品公司'
                if li[-1]=='6':
                    d1['weight'] = 3
                    d1['name'] = '影视名——上映时间'
                if li[-1]=='7':
                    d1['weight'] = 3
                    d1['name'] = '影视名——上映地点'
                if li[-1]=='8':
                    d1['weight'] = 4
                    d1['name'] = '影视名——影视类型'
                if li[-1]=='9':
                    d1['weight'] = 8
                    d1['name'] = '艺术家——艺术家别名'
                if li[-1]=='0':
                    d1['weight'] = 8
                    d1['name'] = '其它'
                relationList.append(d1)

                li_relation.append(dict_relation)
    return li_relation




def home(request):
    """Renders the home page."""
    assert isinstance(request, HttpRequest)
    return render(
        request,
        'app/index.html',
        context_instance = RequestContext(request,
        {
            'title':'Home Page',
            #'year':datetime.now().year,
        })
    )

def contact(request):
    """Renders the contact page."""
    assert isinstance(request, HttpRequest)
    return render(
        request,
        'app/contact.html',
        context_instance = RequestContext(request,
        {
            'title':'Contact',
            'message':'Your contact page.',
            'year':datetime.now().year,
        })
    )

def about(request):
    """Renders the about page."""
    assert isinstance(request, HttpRequest)
    return render(
        request,
        'app/about.html',
        context_instance = RequestContext(request,
        {
            'title':'About',
            'message':'Your application description page.',
            'year':datetime.now().year,
        })
    )

def messagebox(request):
    """Renders the about page."""
    assert isinstance(request, HttpRequest)
    return render(
        request,
        'app/messagebox.html',
        context_instance = RequestContext(request,
        {
            'title':'MessageBox',
            'message':'Your application description page.',
            'year':datetime.now().year,
        })
    )


def RE(request):
    if request.method == 'POST':
        del entityList[:]
        del relationList[:]
        relation_text = json.loads(request.POST['relation_text'])
        li = sentence_seg(relation_text)
        for i in range(len(li)):
            #step2:ner
            FilmNer(li[i])
            #step3:data_combine
            ner_hb.data_combine("ner_result","ner_result_combine")
            #step4:meiju
            ner_meiju.moresents("ner_result_combine","ner_result_moresents")
            #step5:features extraction
            cmd_features_extract = 'feature_extraction ner_result_moresents ner_result_moresents_features'
            os.system(cmd_features_extract)
            #step5:crf
            echars_li=CRF_relation("ner_result_moresents_features")
            #step5:maxent
            #maxent("output.txt","maxent_result")
    return HttpResponse("")


def echarts(request):
    """Renders the about page."""
    assert isinstance(request, HttpRequest)
    return render(
        request,
        'app/echarts.html',
        context_instance = RequestContext(request,
        {
            'title':'Echarts',
            #'message':'Your application description page.',
            #'year':datetime.now().year,
            'entityList':json.dumps(entityList),
            'relationList':json.dumps(relationList),
        })
    )


def search(request):
    if request.method == 'POST':
        entitylabel = json.loads(request.POST['data_name'])
        wiki = Wikiinfotable.objects.filter(entity1 = entitylabel)[0:8]
        w_List =  []
        num = 1
        for w in wiki:
            w_dict={}
            w_dict['relation'] = w.relationtype
            w_dict['entity2'] = w.entity2
            w_List.append(w_dict)
    return HttpResponse(json.dumps(w_List),content_type='application/json')