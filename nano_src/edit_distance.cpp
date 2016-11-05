#include "library.h"
#include "constant.h"
#include "global.h"
#include "structure.h"
#include "functions.h"

cell **matrix = NULL;

int find_kband_similarity(string& str1, string& str2, vector<pair<char, char> >& alignment, int &str1_end, int &str2_end)
{
        int row, column;
        int kband, match, preoff = -1, shift;
        int i, j, h, k, offset;
        float len_ratio;
        string swap;

	/*
        if(str1.length() < str2.length())
        {
                swap = str1;
                str1 = str2;
                str2 = swap;
        }
	*/

	//cout << "str1 = " << str1 << endl;
	//cout << "str2 = " << str2 << endl;

	kband = 5;
	kband = min(FRAGMENTSIZE / 4, str2.length() / 2);//03-26-15//70

        row = str1.length() + 1;
        column = 2 * kband + 2;
        len_ratio = 1.0 * str2.length() / str1.length();
	
        for(i = 0; i < row; i ++)
        {
                matrix[i][0].cost = i * -GAP * LOCAL;// * 0;
                matrix[i][0].dir = UP;
		matrix[i][0].matrix_col = 0;
		matrix[i][0].str2_index = 0;
		matrix[i][0].match = 0;
		matrix[i][0].length = i;
        }

        for(j = 1; j < column; j++)
        {
                matrix[0][j].cost = j * -GAP * LOCAL;
                matrix[0][j].dir = BACK;
		matrix[0][j].matrix_col = j - 1;
		matrix[0][j].str2_index = j;
		matrix[0][j].match = 0;
		matrix[0][j].length = j;
        }

        int max_score = 0.0;
        int max_row, max_col;
	int max_length, max_match;
        max_row = max_col = 0;
	int gap_cost = 0;
	str1_end = str2_end = 0;
	max_length = max_match = 0;
	int start_col, end_col;

        for(i = 1; i < row; i++)
        {
                offset = (int) (len_ratio * i);
		start_col = max(1 - offset, -kband);
		end_col = min(str2.length() - offset, kband);

                for(h = start_col; h <= end_col; h++)
                {
                        k = offset + h;

                        if(k >= 1 && k <= str2.length())
                        {
				if(str1.at(i - 1) == str2.at(k - 1))
				{	
					match = WEIGHT;
						
					if(i > 1 && k > 1)//03-26-15
					{
						if(str1.at(i - 2) == str2.at(k - 2))
						{
							match = WEIGHT + MAT_NEXT;
						}
					}
					
				}
				else
				{
					match = MISMATCH;
											
					if(i > 1 && k > 1)
					{
						if(str1.at(i - 2) == str2.at(k - 2))
							match = MISMATCH - MAT_NEXT;
					}
					
				} 
				
                                if(offset > kband + 1)
                                        j = k - (offset - kband) + 1;
                                else
                                        j = k;

                                if(preoff == offset || offset <= kband + 1)
                                        shift = -1;
                                else
                                        shift = offset - preoff -1;

                                //assert(i >= 1 && i < row);
                                //assert(j >= 1 && j < 2 * kband + 2);

                                matrix[i][j].dir = DIAG;
				matrix[i][j].matrix_col = (j + shift);
				matrix[i][j].str2_index = k;
				matrix[i][j].length = matrix[i - 1][j + shift].length + 1;
					
				if(match >= WEIGHT)
					matrix[i][j].match = matrix[i - 1][j + shift].match + match;//1;
				else
					matrix[i][j].match = matrix[i - 1][j + shift].match;
				/*
				if(match >= WEIGHT)
					matrix[i][j].match = matrix[i - 1][j + shift].match + match;
				*/
                                if(LOCAL == 0)
                                        matrix[i][j].cost = max(0, matrix[i - 1][j + shift].cost + match);
                                else
                                        matrix[i][j].cost = matrix[i - 1][j + shift].cost + match;

                                if(j + shift + 1 <= 2 * kband + 1)
                                {
					if(matrix[i - 1][j + shift + 1].dir == DIAG)
                                   		gap_cost = GAP + GAP_OPEN;
                                	else
                                        	gap_cost = GAP;

                                        if(matrix[i][j].cost < matrix[i - 1][j + shift + 1].cost - gap_cost) 
					{ // <= changed to <
				                matrix[i][j].dir = UP;
						matrix[i][j].matrix_col = (j + shift + 1);
						matrix[i][j].str2_index = k;
                                        	matrix[i][j].cost = matrix[i - 1][j + shift + 1].cost - gap_cost;
						matrix[i][j].length = matrix[i - 1][j + shift + 1].length + 1;
						matrix[i][j].match = matrix[i - 1][j + shift + 1].match;
					}
                                }

                                if(j - 1 >= 1)// && str2.at(k - 1) != 'N')
                                {
					if(matrix[i][j - 1].dir == DIAG)
                                        	gap_cost = GAP + GAP_OPEN;
                                	else
                                        	gap_cost = GAP;

                                        if(matrix[i][j].cost < matrix[i][j - 1].cost - gap_cost)
					{ // <= changed to <
                                                matrix[i][j].dir = BACK;
						matrix[i][j].matrix_col = (j - 1);
						matrix[i][j].str2_index = k;
                                        	matrix[i][j].cost = matrix[i][j - 1].cost - gap_cost;
						matrix[i][j].length = matrix[i][j - 1].length + 1;
						matrix[i][j].match = matrix[i][j - 1].match;
					}
                                }
				/*
                                if(DEBUG == 99)
                                {
					//cout << "@(" << i << "," << j << "," << k << "): " << matrix[i][j] << "\t";
                                        //cout << "@(" << str1.at(i - 1) << ", " << str2.at(k - 1) << "): " << matrix[i][j] << "\t";
                                        cout << "(" << str1.at(i - 1) << "," << str2.at(k - 1) << "):"
                                                << "(" << i << "," << j << "):" << matrix[i][j].cost << "\t";
                                }
				*/
								
                                if(k >= 0.75 * row && i >= 0.75 * row && max_score <= matrix[i][j].cost)
                              	{		
                                       	max_score = matrix[i][j].cost;
                                       	max_row = i;
                                       	max_col = j;
                                       	str1_end = i;
                                       	str2_end = k;
					max_length = matrix[i][j].length;
					max_match = matrix[i][j].match;
					/*
					cout << "Row = " << str1.at(i - 1) << ", and Col = " << 
					str2.at(k - 1) << ", Len " << matrix[i][j].length <<
					", Score = " << matrix[i][j].match << ": " <<
					(1.0 * matrix[i][j].match / matrix[i][j].length)  << endl;
					*/
					//cout << "(" << i << ", " << j << ")    ";
					//cout << "" << i << "," << k << "," << j << "    ";
					//cout << "" << i << "|" << k << "|" << j << "|" << matrix[i][j].dir << "|  ";
					
				}

                        }

                }

                preoff = offset; //to prevent updating same row values
		if(j + 1 < column)
                        matrix[i][j + 1].cost = -(i + j + 1) * GAP * LOCAL;
		/*		
                if(DEBUG == 99) 
		{
                        cout << "\n";
                        cout << "-------------------------------------------------------------------------------";
			cout << "-------------------------------------------------------------------------------" << endl;

                }
		*/
		
        }


	//cout << "Total score = " << matrix[max_row][max_col] << ", with kband_score = " << max_score << endl;
	cout << "Max score found at the row = " << str1_end << ", and at the col = " << str2_end << endl;
	trace_path_cell_back(matrix, max_row, max_col, str1, str2, alignment);
	cout << "" << alignment.size() << " VS " << max_length << endl;
	assert(alignment.size() == max_length);	
	return max_match;
}

int find_banded_similarity(string& str1, string& str2, vector<pair<char, char> >& current_alignment, 
				int &end1, int &end2, bool direction)
{

	float ratio;
	vector<pair<char, char> > sub_alignment;
	//vector<pair<char, char> > current_alignment;

	int sub_alignment_score = 0;
	int sub_alignment_similarity = 0;
	int alignment_quality = 0;
	int str1_len, str2_len;
	int str1_start, str2_start;	
	int str1_end, str2_end;
	int rest_of_str1, rest_of_str2;

	string str1_substr, str2_substr;

	str1_len = str1.length();
	str2_len = str2.length();
	str1_start = str2_start = 0;
	//cout << endl;

	double prescore = 3.20, current_score;	
	double presimilarity = 0.55, current_similarity;
	int alignment_index;
	int total_similarity_score = 0, total_len = 0;
	bool flag = false, break_flag = false;
	bool forward_hack = false;

	//cout << "START: " << "" << str1.length() << " VS " << str2.length() << endl;
	while(str1_start < str1_len && str2_start < str2_len)
	{
		rest_of_str1 = str1.length() - str1_start;
		rest_of_str2 = str2.length() - str2_start;

		//cout << "rest_of_str1 = " << rest_of_str1 << ", and rest_of_str2 = " << rest_of_str2 << endl;

		ratio = 1.0 * rest_of_str2 / rest_of_str1;
		
		if(rest_of_str1 < FRAGMENTSIZE || rest_of_str2 < FRAGMENTSIZE)
		{
			rest_of_str1 = rest_of_str2 = min(rest_of_str1, rest_of_str2);
			if(rest_of_str1 < ANCHOR)
				break;
			flag = true;
		}
		else
			rest_of_str1 = rest_of_str2 = FRAGMENTSIZE;
		
		str1_substr = str1.substr(str1_start, rest_of_str1);
		str2_substr = str2.substr(str2_start, rest_of_str2);		
		//cout << "str1 = " << str1_substr.substr(0, min(100, str1_substr.length())) << endl;
		//cout << "str2 = " << str2_substr.substr(0, min(100, str2_substr.length())) << endl;

		sub_alignment.clear();
		sub_alignment_score = find_kband_similarity(str1_substr, str2_substr, sub_alignment, str1_end, str2_end);
		if(str1_end < ANCHOR || str2_end < ANCHOR)
		{
			//cout << "INCOMPLETE & BREAK" << endl;
			break;
		}
		/*	
		if(str1_start == 0)
		{
			if(direction == true && 1.0 * sub_alignment_score / sub_alignment.size() < 3.60)
				flag = true;
			if(direction == false && 1.0 * sub_alignment_score / sub_alignment.size() < 3.50)
				flag = true;
		}
		*/
	
		if(1.0 * (alignment_quality + sub_alignment_score) / (total_len + sub_alignment.size()) < 3.40)
		{
			//forward_hack = flag = true;
				
			if(str1_start == 0)
				forward_hack = flag = true;
			else
				flag = true;//break;	
			/*	
			if(str1_start == 0 && direction == true)
				forward_hack = flag = true;
			else
				break;
		
			*/
		}


		sub_alignment_similarity = 0;
		for(int i = sub_alignment.size() - 1; i >= 0; i--)
		{
			current_alignment.push_back(sub_alignment[i]);
			if(sub_alignment[i].first == sub_alignment[i].second && sub_alignment[i].first != '-')
				sub_alignment_similarity += 1;//total_score += 1;	
		}


		current_score = 1.0 * sub_alignment_score / sub_alignment.size();
		current_similarity = 1.0 * sub_alignment_similarity / sub_alignment.size();
		if(presimilarity < 0.55 && current_similarity < 0.55)
		{	
			flag = true;
		}

		prescore = current_score;
		presimilarity = current_similarity;
		
		total_len += sub_alignment.size();
		alignment_quality += sub_alignment_score;
		total_similarity_score += sub_alignment_similarity;

		str1_start += str1_end;
		str2_start += str2_end;

		if(100.00 * total_similarity_score / total_len < 1.01 * KBAND_PERCENT_MATCH)
		{
			//cout << "breaking at the matching ratio = " << (100.00 * total_score / total_len) << endl;
			flag = true;//break;
		}

	
		if(flag == true)
		{
			int i, k;
			float score = 0.59;
			
			if(forward_hack == true)
				score = 0.65;

			for(k = 0; k < sub_alignment.size(); k++)
			{
				if(sub_alignment[k].first == sub_alignment[k].second && sub_alignment[k].first != '-')
				{
					if(1.0 * sub_alignment_similarity / (sub_alignment.size() - k) < score)
					{
						sub_alignment_similarity -= 1;
						total_similarity_score -= 1;
					}	
					else
						break;
				}
				
				if(sub_alignment[k].first != '-')
					str1_start -= 1;
				if(sub_alignment[k].second != '-')
					str2_start -= 1;
				current_alignment.pop_back();
				total_len -= 1;
			}

			break;
		}
	}

	end1 = str1_start;
	end2 = str2_start;
	return total_similarity_score;
}


