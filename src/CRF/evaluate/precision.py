#coding:utf-8
li1=[]
li=[]
with open("keys.txt") as fin:
    for line in fin:
        print line.strip()
        li1.append(line.strip())
with open("../result/result.txt") as fin2:
    for line in fin2:
        print line.replace('\n','').split('\t')[-1]
        li.append(line.replace('\n','').split('\t')[-1])


lines = 0
mingzhong = 0

lines_1 = 0
lines_2 = 0
lines_3 = 0
lines_4 = 0
lines_5 = 0
lines_6 = 0
lines_7 = 0
lines_8 = 0
lines_9 = 0
lines_0 = 0

mingzhong_1 = 0
mingzhong_2 = 0
mingzhong_3 = 0
mingzhong_4 = 0
mingzhong_5 = 0
mingzhong_6 = 0
mingzhong_7 = 0
mingzhong_8 = 0
mingzhong_9 = 0
mingzhong_0 = 0

for i in range(len(li1)):
    lines+=1
    if li1[i] == li[i]:
        mingzhong+=1
    if li1[i]=='1':
        lines_1+=1
        if li[i]=='1':
            mingzhong_1+=1 
    if li1[i]=='2':
        lines_2+=1
        if li[i]=='2':
            mingzhong_2+=1
    if li1[i]=='3':
        lines_3+=1
        if li[i]=='3':
            mingzhong_3+=1
    if li1[i]=='4':
        lines_4+=1
        if li[i]=='4':
            mingzhong_4+=1
    if li1[i]=='5':
        lines_5+=1
        if li[i]=='5':
            mingzhong_5+=1
    if li1[i]=='6':
        lines_6+=1
        if li[i]=='6':
            mingzhong_6+=1
    if li1[i]=='7':
        lines_7+=1
        if li[i]=='7':
            mingzhong_7+=1
    if li1[i]=='8':
        lines_8+=1
        if li[i]=='8':
            mingzhong_8+=1
    if li1[i]=='9':
        lines_9+=1
        if li[i]=='9':
            mingzhong_9+=1
    if li1[i]=='0':
        lines_0+=1
        if li[i]=='0':
            mingzhong_0+=1

print("%f" % (1.0*mingzhong/lines))
print("1:%f" % (1.0*mingzhong_1/lines_1))
print("2:%f" % (1.0*mingzhong_2/lines_2))
print("3:%f" % (1.0*mingzhong_3/lines_3))
print("4:%f" % (1.0*mingzhong_4/lines_4))
print("5:%f" % (1.0*mingzhong_5/lines_5))
print("6:%f" % (1.0*mingzhong_6/lines_6))
print("7:%f" % (1.0*mingzhong_7/lines_7))
print("8:%f" % (1.0*mingzhong_8/lines_8))
print("9:%f" % (1.0*mingzhong_9/lines_9))
print("0:%f" % (1.0*mingzhong_0/lines_0))
