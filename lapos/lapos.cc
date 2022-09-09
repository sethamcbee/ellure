/*
 * $Id$
 */

#include <stdio.h>
#include <fstream>
#include <map>
#include <list>
#include <set>
#include <iomanip>
#include <iostream>
#include <cfloat>
#include <sstream>
//#include "maxent.h"
#include "crf.h"
#include "common.h"
#include <sys/time.h>

using namespace std;

bool PERFORM_TOKENIZATION = false;
bool OUTPUT_TAG_PROBS = false;
bool STANDOFF = false;
bool UIMA     = false;
bool ENJU     = false;
int  NBEST = 0;

static CRF_Model crfm;

string MODEL_DIR = "."; // the default directory for saving the models

const double PROB_OUTPUT_THRESHOLD = 0.001; // suppress output of tags with a very low probability

void
tokenize(const string & s, vector<Token> & vt, const bool use_upenn_tokenizer);

void
crf_decode_lookahead(Sentence & s, CRF_Model & m, vector< map<string, double> > & tagp);

void read_WordNet(const string & wordnetdir);


ParenConverter paren_converter;

/*
int push_stop_watch()
{
  static struct timeval start_time, end_time;
  static bool start = true;
  if (start) {
    gettimeofday(&start_time, NULL);
    start = false;
    return 0;
  }

  gettimeofday(&end_time, NULL);
  int elapse_msec = (end_time.tv_sec - start_time.tv_sec) * 1000 +
    (int)((end_time.tv_usec - start_time.tv_usec) * 0.001);
  cerr << elapse_msec << " msec" << endl;
  start = true;
  return elapse_msec;
}
*/


void print_help()
{
  cout << "Lookahead POS Tagger, a trainable part-of-speech tagger." << endl;
  cout << "Usage: lapos [OPTION]... [FILE]" << endl;
  cout << "Annotate each word in FILE with a part-of-speech tag." << endl;
  cout << "By default, the tagger assumes that FILE contains one sentence per line" << endl;
  cout << "and the words are tokenized with white spaces." << endl;
  //  cout << "Try -t and -s options if you want the tagger to process raw text." << endl;
  cout << "Use -t option if you want to process untokenized sentences." << endl;
  cout << endl;
  cout << "Mandatory arguments to long options are mandatory for short options too." << endl;
  cout << "  -m, --model=DIR        specify the directory containing the models" << endl;
  cout << "  -t, --tokenize         perform tokenization" << endl;
  //  cout << "  -s, --split-sentence   perform sentence-splitting" << endl;
  cout << "  -s, --standoff         output in stand-off format" << endl;
  cout << "  -u, --uima             output in UIMA format" << endl;
  cout << "  -e, --enju             output in Enju format" << endl;
  cout << "  -h, --help             display this help and exit" << endl;
  cout << endl;
  cout << "With no FILE, or when FILE is -, read standard input." << endl;
  cout << endl;
  cout << "Report bugs to <tsuruoka@gmail.com>" << endl;
}

void print_version()
{
  cout << "POS tagger 0.1" << endl;
}

struct TagProb
{
  string tag;
  double prob;
  TagProb(const string & t_, const double & p_) : tag(t_), prob(p_) {}
  bool operator<(const TagProb & x) const { return prob > x.prob; }
};

void lapos_init()
{
    MODEL_DIR = "lapos/model_wsj02-21";
    if (MODEL_DIR[MODEL_DIR.size()-1] != '/')
    {
        MODEL_DIR += "/";
    }
    if (!crfm.load_from_file(MODEL_DIR + "model.la", false))
    {
        exit(1);
    }
}

std::string lapos_main(const string& input_string)
{
  string WORDNET_DIR = "";
  
  string ifilename;
  PERFORM_TOKENIZATION = true;

  stringstream input_sstream{input_string};
  istream *is(&input_sstream);

  stringstream output;

  string line;
  int nlines = 0;
  while (getline(*is, line)) {
    nlines++;
    vector<Token> vt;
    tokenize(line, vt, PERFORM_TOKENIZATION);

    if (vt.size() > 990) {
      cerr << "warning: the sentence is too long. it has been truncated." << endl;
      while (vt.size() > 990) vt.pop_back();
    }

    // convert parantheses
    vector<string> org_strs;
    for (vector<Token>::iterator i = vt.begin(); i != vt.end(); i++) {
      org_strs.push_back(i->str);
      i->str = paren_converter.Ptb2Pos(i->str);
      i->prd = "?";
    }

    if (STANDOFF) cout << line << endl;
    if (vt.size() == 0) { cout << endl; continue; }

    // tag the words
    vector< map<string, double> > tagp0, tagp1;
    //    crf_decode_forward_backward(vt, crfm, tagp0);
    crf_decode_lookahead(vt, crfm, tagp0);
    if (false) {
      //      ef_decode_beam(vt, vme, tagp1);
      assert(0); exit(1);
    } else {
      for (vector<Token>::const_iterator i = vt.begin(); i != vt.end(); i++) {
	map<string, double> dummy;
	tagp1.push_back(dummy);
      }
    }

    // merge the outputs (simple interpolation of probabilities)
    vector< map<string, double> > tagp; // merged
    //if(tagp0.size() > 0) {
    for (size_t i = 0; i < vt.size(); i++) {
      const map<string, double> & crf = tagp0[i];
      const map<string, double> & ef  = tagp1[i];
      map<string, double> m, m2; // merged

      double sum = 0;
      for (map<string, double>::const_iterator j = crf.begin(); j != crf.end(); j++) {
	//	cout << j->first << ":" << j->second << " ";
	m.insert(pair<string, double>(j->first, j->second));
	sum += j->second;
      }
      //      cout << endl;

      for (map<string, double>::const_iterator j = ef.begin(); j != ef.end(); j++) {
	//	cout << j->first << ":" << j->second << " ";
	sum += j->second;
	if (m.find(j->first) == m.end()) {
	  m.insert(pair<string, double>(j->first, j->second));
	} else {
	  m[j->first] += j->second;
	}
      }
      //      cout << endl;

      const double th = PROB_OUTPUT_THRESHOLD * sum;
      for (map<string, double>::iterator j = m.begin(); j != m.end(); j++) {
	if (j->second >= th) m2.insert(*j);
      }
      double maxp = -1;
      string maxtag;
      for (map<string, double>::iterator j = m2.begin(); j != m2.end(); j++) {
	const double p = j->second;
	if (p > maxp) { maxp = p; maxtag = j->first; }
	//	cout << j->first << ":" << j->second << " ";
      }
      //      cout << endl;
      tagp.push_back(m2);
      vt[i].prd = maxtag;
    }
    //}
    // print the resutls
    for (size_t i = 0; i < vt.size(); i++) {
      const string s = org_strs[i];
      const string p = vt[i].prd;
      if (STANDOFF || OUTPUT_TAG_PROBS || UIMA || ENJU) {
	if (STANDOFF || UIMA || ENJU) {
	  output << vt[i].begin << "\t" << vt[i].end;
	  if (!UIMA && !ENJU){
	    output << "\t";
	  }
	}
	if (!UIMA && !ENJU){
	  output << s;
	}
	if (OUTPUT_TAG_PROBS) {
	  vector<TagProb> tp;
	  double sum = 0;
	  for (map<string, double>::iterator j = tagp[i].begin(); j != tagp[i].end(); j++) {
	    tp.push_back(TagProb(j->first, j->second));
	    sum += j->second;
	  }
	  sort(tp.begin(), tp.end());
	  for (vector<TagProb>::iterator j = tp.begin(); j != tp.end(); j++) {
	    const double p = j->prob / sum; // normalize
	    if (p == 1) output << resetiosflags(ios::fixed);
	    else        output << setiosflags(ios::fixed) << setprecision(3);
	    output << "\t" << j->tag << "\t" << p;
	  }
	} else {
	  output << "\t" + p;
          if (ENJU) {
            output << "\t1";
          }
	}
	if (UIMA){
	  output << "\t0";
	}
	output << endl;
      } else {
	if (i == 0) output << s + "/" + p;
	else        output << " " + s + "/" + p;
      }
    }
    output << endl;
    crfm.incr_line_counter();
  }
  
  return output.str();
}

/*
 * $Log$
 */

