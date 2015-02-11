#include <random>
#include <iostream>
#include <string>
#include <map>
#include <iomanip>
#include <vector>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <sstream>

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_real_distribution<> uniform_distr(0,1);

class clutch
{
public:
	int father;
	int size;
};

class male
{
public:
	int real_MS;
	int real_RS;
	int sample_MS;
	int sample_RS;
};

int neg_binomial_brute(int successes, const double prob_success)
{
	int number_successes, number_failures;
	number_successes = 0;
	number_failures = 0;
	double drnum;
	while (number_successes < successes)
	{
		drnum = uniform_distr(gen);
		if (drnum < prob_success)
			number_successes++;
		else
			number_failures++;
	}

	return number_failures;
	
}

int binomial_brute(int tries, const double prob)
{
	int number_successes = 0;
	double drnum;

	for (int i = 0; i < tries; i++)
	{
		drnum = uniform_distr(gen);
		if (drnum < prob)
			number_successes++;
	}

	return number_successes;
}

void test(const int t, const double p, const int& s, int* msarray, bool use_binomial) 
{
	// t is the number of trials
	// p is the probability of success
	// s is the sample size

	int bdrn;

	std::cout << std::endl;
	std::cout << "p == " << p << std::endl;
	std::cout << "t == " << t << std::endl;
	if (use_binomial)
		std::cout << "binomial distribution!\n";
	else
		std::cout << "neg. binomial dist.!\n";

	// generate the distribution as a histogram
	std::map<int, int> histogram;
	for(int i = 0; i < s; i++)
	{
		if (use_binomial)
			bdrn = binomial_brute(t,p);
		else
			bdrn = neg_binomial_brute(t,p);
		++histogram[bdrn];
		msarray[i] = bdrn;
	}

	// print results
	std::cout << "Histogram for " << s << " samples:" << std::endl;
	for (const auto& elem : histogram)
	{
		std::cout << std::setw(5) << elem.first << ' ' << std::string(elem.second, ':') << std::endl;
	}
}

int main()
{
	int i, j;
	int rep_number;
	int rep_loop;

	int t_dist = 1;
	double p_dist = 0.5;
	int samples = 10;

	double mean_clutch = 300;
	double std_clutch = 50;
	int empirical_sample_size = 10;
	bool flag_binomial;
	bool flag_shallow_slope;

	std::random_device ra_dev;
	std::mt19937 norm_gen(ra_dev());
	std::normal_distribution<> norm_distr(mean_clutch, std_clutch);

	std::string filename;
	std::string filename_plus;
	std::string string_rep;
	std::stringstream temp_fn;
	std::string query;

	std::cout << "(B)inomial or (N)egative binomial distribution?";
	std::cin >> query;
	if (query[0] == 'b' || query[0] == 'B')
		flag_binomial = true;
	else
		flag_binomial = false;
	std::cout << "(S)teep or (z)ero slope for Bateman gradient?";
	std::cin >> query;
	if (query[0] == 'z' || query[0] == 'Z')
		flag_shallow_slope = true;
	else
		flag_shallow_slope = false;
    std::cout << "Enter an integer value for trials (binomial) or successes (neg_binomial) distribution (where 0 <= t): ";
    std::cin >> t_dist;
    std::cout << "Enter a double value for success probability (where 0.0 <= p <= 1.0): ";
    std::cin >> p_dist;
    std::cout << "Enter an integer value for a sample count (total number of males in pop): ";
    std::cin >> samples;
	std::cout << "Enter an integer value for the empirical sample size (number of clutches sampled): ";
    std::cin >> empirical_sample_size;
	std::cout << "Enter filename: ";
	std::cin >> filename;
	std::cout << "Number of Reps: ";
	std::cin >> rep_number;
		
	int* MS = new int[samples];

	for(rep_loop = 0; rep_loop < rep_number; rep_loop++)
	{
		for (i = 0; i < samples; i++)
			MS[i] = 0;
		test(t_dist, p_dist, samples, MS, flag_binomial);
		temp_fn.str("");
		temp_fn << filename << "_rep" << rep_loop << ".txt";
		filename_plus = temp_fn.str();

		std::cout << "\n\nID\tMS\tRS\n";
		for (i = 0; i < samples; i++)
			std::cout << i << "\t" << MS[i] << "\n";

		double MSmean, MSvar, dsamples;
		MSmean = 0;
		for (i = 0; i < samples; i++)
			MSmean = MSmean + MS[i];
		dsamples = static_cast<double>(samples);
		MSmean = MSmean/dsamples;

		MSvar = 0;
		double tempMS;
		for (i = 0; i < samples; i++)
		{
			tempMS = static_cast<double>(MS[i]);
			MSvar = MSvar + (tempMS - MSmean)*(tempMS - MSmean);
		}
		MSvar = MSvar/(dsamples-1);

		std::cout << "\nMeanMS:\t" << MSmean;
		std::cout << "\nVarMS:\t" << MSvar;

		std::vector<clutch> clutches;
		clutch temp_clutch;

		int numberClutches = 0;
		double dtempCS, dtempMS;
		int itempCS;
		for (i = 0; i < samples; i++)
		{
			for (j = 0; j < MS[i]; j++)
			{
				temp_clutch.father = i;
				dtempCS = norm_distr(norm_gen);
				if (flag_shallow_slope)
				{
					dtempMS = MS[i];
					dtempCS = dtempCS/dtempMS;
				}
				itempCS = static_cast<int>(std::floor(dtempCS+0.5));
				temp_clutch.size = itempCS;
				clutches.push_back(temp_clutch);
				numberClutches++;
			}
		}


		if (rep_number == 1)
		{
			std::cout << "\n\nEnter a character to continue:";
			std::cin >> i;
		}

		std::cout << "Clutches:\nSire\tSize\n";
		for (i = 0; i < numberClutches; i++)
		{
			std::cout << clutches[i].father << "\t" << clutches[i].size << "\n";
		}

		if (rep_number == 1)
		{
			std::cout << "\n\nEnter a character to continue:";
			std::cin >> i;
		}

		// draw a sample from the clutches without replacement
		// most easily done by shuffling the vector

		std::shuffle(clutches.begin(),clutches.end(),norm_gen);

		std::cout << "\nShuffled clutches:\nSire\tSize\n";
		for (i = 0; i < numberClutches; i++)
		{
			std::cout << clutches[i].father << "\t" << clutches[i].size << "\n";
		}

		// Now that they are shuffled, just take the first however many for the sample

		if (empirical_sample_size > numberClutches)
			empirical_sample_size = numberClutches;

		// All we need to do is calculate the male ID, MS, and RS in the population and sample.

		std::vector<male> males;
		male temp_male;
		temp_male.real_MS = 0;
		temp_male.real_RS = 0;
		temp_male.sample_MS = 0;
		temp_male.sample_RS = 0;
		for (i = 0; i < samples; i++)
			males.push_back(temp_male);

		for (i = 0; i < empirical_sample_size; i++)
		{
			males[clutches[i].father].sample_MS++;
			males[clutches[i].father].sample_RS = males[clutches[i].father].sample_RS + clutches[i].size; 
		}

		for (i = 0; i < numberClutches; i++)
		{
			males[clutches[i].father].real_MS++;
			males[clutches[i].father].real_RS = males[clutches[i].father].real_RS + clutches[i].size;
		}

		std::cout << "\n\nReal\nID\tMS\tRS\n";
		for (i = 0; i < samples; i++)
			std::cout << i << "\t" << males[i].real_MS << "\t" << males[i].real_RS << "\n";

		std::cout << "\n\nSample\nID\tMS\tRS\n";
		for (i = 0; i < samples; i++)
			std::cout << i << "\t" << males[i].sample_MS << "\t" << males[i].sample_RS << "\n";


		// Finally just save the population and sample to a file

		std::ofstream outfile;
		outfile.open(filename_plus);

		outfile << "Real Population:\n";
		outfile << "MS\tRS\n";
		for (i = 0; i < samples; i++)
			outfile << males[i].real_MS << "\t" << males[i].real_RS << "\n";
		outfile << "\nSample:\nMS\tRS\n";
		for (i = 0; i < samples; i++)
			outfile << males[i].sample_MS << "\t" << males[i].sample_RS << "\n";
		outfile << "\nSample, no zeros:\nMS\tRS\n";
		for (i = 0; i < samples; i++)
			if (males[i].sample_MS > 0)
				outfile << males[i].sample_MS << "\t" << males[i].sample_RS << "\n";

		outfile.close();

	} // rep_loop

	std::cout << "\nEnter a character to exit\n";
	std::cin >> i;

	delete[] MS;
}