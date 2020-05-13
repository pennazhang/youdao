#version 0.0.0.2
import time
import re
from selenium import webdriver

#Wait some time for website to be ready (in seconds)
DELAY_SECOND_FOR_READY = 3;

pageCount = 0;
MAX_PAGE_COUNT = 8;
pageList = [];
for i in range(MAX_PAGE_COUNT):
    pageList.append("");

driver = webdriver.Edge();

key1 = 'p-english family-english size-english';
key2 = 'p-chinese family-chinese size-chinese';

key3 = '<div class="base-top-voice">'
key4 = '<h1 class="base-word abbr chinese change-base">'

inputFile = open('wordlist.txt');
outputFile = open('wordList_result.txt', 'w', encoding='utf-8');
wordIndex = 0;
for line in inputFile:
    wordIndex = wordIndex + 1;
    lineStrip = line.strip('\n').strip();
    if (lineStrip == ''):
        print("Get word %d :" %(wordIndex));
        continue;
    outputFile.write(lineStrip);
    url = 'http://www.iciba.com/' + lineStrip;
#   print("*******************" + url + "************************");
#   outputFile.write(url + '\n');
  
    count = 0;
    findFlag = False;
    while ((findFlag == False) and (count < 1)):
        count = count + 1;
        
        driver.get(url);
        time.sleep(DELAY_SECOND_FOR_READY);
        html = driver.page_source;
#        print(html);
        
        #begin of getting the word translation.
        list3=[i.start() for i in re.finditer(key3, html)]
        list4=[i.start() for i in re.finditer(key4, html)]
        if ((len(list3) == 1) and (len(list4) == 1)):
            key3Position = int(list3[0]);
            key4Position = int(list4[0]);
            newLine1 = html[key3Position : key4Position];
            deleteKey1 = re.compile('<.*?>');   # delete all content in <*****>
            newLine2 = deleteKey1.sub('', newLine1).replace(' ','').replace('\r', '').replace('\n', '');
            outputFile.write(' : ' + newLine2 + '\n');
        elif ((len(list3) == 1) and (len(list4) == 0)):
#            print("Get word %d : %s explain error, key3 = %d, key4 = %d\n" %(wordIndex, lineStrip, len(list3), len(list4)));
            key3Position = int(list3[0]);
#            print(key3Position);
            key4Position = html[key3Position:].find('</ul>');
#            print(key4Position);
            newLine1 = html[key3Position : key3Position + key4Position];
#            print(newLine1);
            deleteKey1 = re.compile('<.*?>');   # delete all content in <*****>
            newLine2 = deleteKey1.sub('', newLine1).replace(' ','').replace('\r', '').replace('\n', '');
            outputFile.write(' : ' + newLine2 + '\n');          
        else:
            #we didn't find the chinese translation.
            print("Get word %d : %s explain error, key3 = %d, key4 = %d\n" %(wordIndex, lineStrip, len(list3), len(list4)));
            outputFile.write(" ::\n");
        #end of getting the word translation.
        
        list = html.split('\n');
        for htmlLine in list:
            if htmlLine.find(key1) > 0:
                if htmlLine.find(key2) > 0:
                    findFlag = True;
                    break;
        
        if (findFlag == True):  
            #we have found the corrent line.
            newLine = htmlLine.replace(key1, key2).replace(key2, '\n');
            list = newLine.split('\n');
            if (len(list) < 7):
                findFlag = False;
#               print(newLine);
                print("Get word %d : %s failed for time: %d: list count = %d" %(wordIndex, lineStrip, count, len(list)));
                outputFile.write('\n\n\n');
                continue;
            
            index = 1;
            reKey1 = re.compile('</p>.*');  # delete all content after </p>
            reKey2 = re.compile('<.*?>');   # delete all content in <*****>
            reKey3 = re.compile('">\s*');   # delete all "\"> " 
            while (index < 7):
#               print(list[index]);
                result1 = reKey1.sub('', list[index]);
#               print(result1);
                result2 = reKey2.sub('', result1);
#               print(result2);
                result3 = reKey3.sub('', result2);
#               print(result3);
                outputFile.write(result3 + '\n');
                index = index + 1;
            outputFile.write('\n');
            print("Get word %d : %s OK." %(wordIndex, lineStrip));
            
            pageList[pageCount] = lineStrip;
            pageCount = pageCount + 1;
            if (pageCount >= MAX_PAGE_COUNT):
#               title = "**************** %s **************\n" %(pageList);
#               outputFile.write(title);
                pageCount = 0;
        else:
            print("Get word %d : %s failed for time: %d: no key1 or key2 if found." %(wordIndex, lineStrip, count));
            outputFile.write('\n\n\n');
#    break;     # we just test 1 word for debugging.
 
#if (pageCount != 0):
#    title = "%s\n" %(pageList[0:pageCount]);
#    outputFile.write(title);
driver.quit()


inputFile.close();
outputFile.close();