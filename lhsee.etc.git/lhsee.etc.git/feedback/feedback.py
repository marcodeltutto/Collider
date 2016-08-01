import os
from ROOT import *
from array import array
import atlasStyle

gROOT.SetStyle("atlasStyle") 
gROOT.ForceStyle() 
gROOT.SetBatch(1) 
canvas = TCanvas("c1","c1",1024,768) 

class response:
    def __init__(self,lines):
        self.q1 = lines[0]
        self.q2 = lines[1]
        self.q3 = lines[2]
        self.more_study = lines[3]
        self.age_answer = lines[4]
        self.q6 = lines[5]
        self.other = lines[6]
        
        self.q1_format = [
        (["Very","Tr","","","",],"Very Interested"),
        (["Fairly","Relativement int","","","",],"Fairly Interested"),
        (["Not at all","Pas int","","","",],"Not Interested"),
        ]
        
        self.q234_answers = [
        (["Strongly agree"  ],"Strongly agree"),
        (["Agree"  ],"Agree"),
        (["Neither" ],"Neither agree or disagree"),
        (["Dont know"  ],"Don't know"),
        (["Disagree" ],"Disagree"),
        ]
        
        self.ages = ["11-16 years","17-18 years","19-21 years","22-35 years","36-50 years","51+ years"]

checks = ["11-16","17-18","19-21","22-35","36-50","51+"]

def check_for_responses(all_lines,responses):
    for iLine,line in enumerate(all_lines):
        for check in checks:
            if line.count(check):
                responses.append(response((all_lines[iLine-4],
                                            all_lines[iLine-3],
                                            all_lines[iLine-2],
                                            all_lines[iLine-1],
                                            all_lines[iLine],
                                            all_lines[iLine+1],
                                            all_lines[iLine+2])))
def checkAges(responses):
    age_hist = {}
    for age in responses[0].ages: age_hist[age] = 0.0
    for _response in responses:
        for _age in _response.ages:
            if _response.age_answer.count(_age): age_hist[_age] +=1

    for age in age_hist.keys():
        age_hist[age]/=len(responses)
        print age,":",age_hist[age]
    return age_hist

def under18studyPhysics(responses):
    #setup dict for answers
    studyMore = {}
    for _answer in responses[0].q234_answers:
        studyMore[_answer[1]] = 0.0
    

    for _response in responses:
        
        for _age in _response.ages:
            #check age < 18
            if _response.age_answer.count("11-16") or _response.age_answer.count("17-18"):
                #check which response
                for _answer in _response.q234_answers:
                    #if _answer[0][0].count(_response.more_study):
                    if _response.more_study.count(_answer[0][0]):
                        studyMore[_answer[1]]+=1.0
                        break
    #normalise
    tot_responses =0.0
    for val in studyMore.values(): tot_responses +=val
    for answer in studyMore.keys(): studyMore[answer] /= tot_responses 
    
    print 'StudyMore:'
    print studyMore
    return studyMore
                    
def freeFormAnswers(responses):
    freeFormResponses = []
    for _response in responses: 
        #freeFormResponses.append(_response.other.lstrip("<other>").rstrip("</other>"))
        freeFormResponses.append(_response.other)
    print freeFormResponses[0],freeFormResponses[1]
    return freeFormResponses




chart_colors = [7,2,3,4,5,6,8]

def makePieChart(quantity_dict,title,fileName,offset):
    chart_inputs = []
    slice_labels = []
    for i in  sorted(quantity_dict.keys()): 
        chart_inputs.append(quantity_dict[i])
        slice_labels.append(i)
    #keys,chart_inputs = quantity_dict.iteritems()
    print chart_inputs
    chart_input = array('d',chart_inputs) 
    colors_input = array('i',chart_colors[:len(chart_input)-1])
    print len(chart_input)
    pieChart = TPie("test",str(title),len(chart_input),chart_input,colors_input)
    pieChart.SetTitle(title)
    for iSlice in xrange(len(chart_inputs)): pieChart.GetSlice(iSlice).SetTitle(slice_labels[iSlice])
    pieChart.SetAngularOffset(offset)
    pieChart.Draw("3d t nol")
    canvas.SaveAs(fileName)


if __name__ == "__main__" :
    
    mails = open('data/all_mails.txt',"r")
    mail_lines = mails.readlines()
    mails.close()
    responses = []
    check_for_responses(mail_lines,responses)
    
    print 'Have ',len(responses),' responses.'


    age_hist = checkAges(responses) 
    makePieChart(age_hist,"test title","age_piechart4.eps",60.)


    regions_dict = {}
    regions_dict['UK'] = 0.15
    regions_dict["Germany"] = 0.13
    regions_dict["US"] = 0.42
    regions_dict["Canada"] = 0.03
    regions_dict["Other"] = 0.27
    makePieChart(regions_dict,str("test title"),"regions_piechart.jpg",70.)

    studyMore_u18_dict = under18studyPhysics(responses)
    makePieChart(studyMore_u18_dict,str("test title"),"studyMore_u18_piechart.jpg",70.)

    freeResponses = freeFormAnswers(responses)
    freeFile = file("freeFormAnswers.txt","w")
    for freeResponse in freeResponses: freeFile.write(freeResponse+"\n")
    freeFile.close()


