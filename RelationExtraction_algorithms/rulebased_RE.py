#coding:utf-8
import sys 
reload(sys) 
sys.setdefaultencoding('utf-8')
import re


def Match_seeds():
    seeds_num = raw_input('请输入种子数目：'.encode('gbk'))
    try:
        seeds_num = int(seeds_num)
        for i in range(1,seeds_num + 1):
            print '请输入种子'.encode('gbk'),i
            e1 = raw_input('entity1:')
            e2 = raw_input('entity2:')
            print '种子%d为：%s\t%s\n'.encode('gbk') % (i,e1,e2)
            seeds_file_str = "seeds"+str(i)+".txt"
            fout = open(seeds_file_str,'w')
            fin = open("./raw_sents.txt")
            line = fin.readline()
            while(line):
                try:
                    raw_pattern = ".*"+e1+".*"+e2+".*"
                    xx = raw_pattern.decode('gbk') 
                    temp = line.decode('utf-8')
                    pattern = re.compile(xx)
                    resu = pattern.findall(temp)
                    for r in resu:
                        fout.write(r.encode('utf-8').replace(e1.decode('gbk').encode('utf-8'),'.*').replace(e2.decode('gbk').encode('utf-8'),'.*') + '\n')     
                    line = fin.readline()
                except:
                    #print 'error'
                    p=0
            fin.close()
            fout.close()
    except Exception,e:
        print e



    
#while(True):
#    fin = open("./raw_sents.txt")
#    line = fin.readline()
#    raw_pattern = raw_input("pattern: ")
#    fout =open('E:/demo/film_relation/relation.txt','w')
#    flog =open('log.csv','a')
#    pattern_sentencess = 0
#    while(line):
#        try:
#            xx = raw_pattern.decode('gbk')
#            temp = line.decode('utf-8')
#            pattern = re.compile(xx)
#            resu = pattern.findall(temp)

#            for r in resu:
#               fout.write( r.encode('utf-8')+'\n')
#               pattern_sentencess=pattern_sentencess + 1
#            line = fin.readline()
#        except:
#            print 'error'
#    print pattern_sentencess
#    flog.write(raw_pattern+','+str(pattern_sentencess)+'\n')
#    flog.close()
#    fout.close()
#    fin.close()
  
if __name__ == "__main__":
    Match_seeds()   

