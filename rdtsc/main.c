#include <stdio.h>
#include <stdlib.h>
#include "rdtsc.h"
#include "../gene.h"
#include <string.h>

#define MAX 31000
#define MAX_LINE 150
#define MAX_LOOP 500

void load_gene(char *filename, char *seq_char)
{
	char temp[MAX_LINE] = "";
	FILE *file;
	file = fopen(filename, "r");
	if(!file)
		printf("ERROR: load_gene: cannot open file %s\n", filename);

	if(file)
    {
		fgets(temp, MAX_LINE, file); 
        while(fgets(temp, MAX_LINE, file) != NULL)
        {
        	strcat(seq_char, temp);
        }
    }
	//fscanf(file, "%s", seq_char);

	fclose(file);
}

int main(int argc, char *argv[])
{
	unsigned long long before = 0;
	unsigned long long after = 0;
	double elapsed = 0.0;

	char seq_char[MAX] = "";
	load_gene("../fastas/LC528232.1.fasta", seq_char);
	// printf("%s", seq_char);
	unsigned long long seq_char_size = strlen(seq_char);

	char seq_char2[MAX] = "";
	load_gene("../fastas/MN908947.3.fasta", seq_char2);
	// printf("%s", seq_char2);
	unsigned long long seq_char_size2 = strlen(seq_char2);


	// Variable used for function
	unsigned short int *seq_short = NULL;
	unsigned short int *seq_short2 = NULL;
	float cms = 0;
	char *rna_seq_short = NULL;
	char *aa_seq_short = NULL;
	
	//TODO : problème avec ça déjà
	seq_short2 = convert_to_binary(seq_char2, 2 * seq_char_size2);
  	
  	gene_map_t g;
    g.gene_start = malloc(sizeof(*g.gene_start) * seq_char_size * 2);
    g.gene_end = malloc(sizeof(*g.gene_end) * seq_char_size * 2);
    
    mutation_map m;
	m.size = malloc(sizeof(unsigned long) * 5);
	m.start_mut = malloc(sizeof(unsigned long) * 5);
	m.end_mut = malloc(sizeof(unsigned long) * 5);
	for(int i = 0; i < 5; i++)
	{
		m.size[i]=0;
    	m.start_mut[i]=0;
    	m.end_mut[i]=0;   	
    }


	/*-----convert_to_binary-----*/
	// for(int i = 0; i < MAX_LOOP; i++)
	// {
 //    	before = rdtsc();
 //    	seq_short = convert_to_binary(seq_char, 2 * seq_char_size);
 //    	after = rdtsc();

 //    	elapsed += (double)(after - before);
	// }
	// printf("convert_to_binary : %lf cycles\n", elapsed / MAX_LOOP);
	// elapsed = 0;
	
	// /*-----detecting_genes-----*/
	// // for(int i = 0; i < MAX_LOOP; i++)
	// // {
	// // 	before = rdtsc();
	// // 	detecting_genes(seq_short, 2 * seq_char_size, &g);
	// // 	after = rdtsc();

	// // 	elapsed += (double)(after - before);
	// // }
	// // printf("detecting_genes : %lf cycles\n", elapsed / MAX_LOOP);
	// // elapsed = 0;

	// /*-----detecting_mutations-----*/
	// for(int i = 0; i < MAX_LOOP; i++)
	// {
 //    	before = rdtsc();
 //    	detecting_mutations(seq_short, 2 * seq_char_size, m);;
 //    	after = rdtsc();

 //    	elapsed += (double)(after - before);
	// }
	// printf("detecting_mutations : %lf cycles\n", elapsed / MAX_LOOP);
	// elapsed = 0;

	// /*-----calculating_matching_score-----*/
	// for(int i = 0; i < MAX_LOOP; i++)
	// {
 //    	before = rdtsc();
	// 	cms = calculating_matching_score(seq_short, 2 * seq_char_size, seq_short2, 2 * seq_char_size2);
 //    	after = rdtsc();

 //    	elapsed += (double)(after - before);
	// }
	// printf("calculating_matching_score : %lf cycles\n", elapsed / MAX_LOOP);
	// elapsed = 0;

	// /*-----generating_mRNA-----*/
	// for(int i = 0; i < MAX_LOOP; i++)
	// {
 //    	before = rdtsc();
	// 	rna_seq_short = generating_mRNA(seq_short, 2 * seq_char_size);
 //    	after = rdtsc();

 //    	elapsed += (double)(after - before);
	// }
	// printf("generating_mRNA : %lf cycles\n", elapsed / MAX_LOOP);
	// elapsed = 0;

	// /*-----generating_amino_acid_chain-----*/
	// for(int i = 0; i < MAX_LOOP; i++)
	// {
 //    	before = rdtsc();
	// 	aa_seq_short = generating_amino_acid_chain(seq_short, 2 * seq_char_size);
 //    	after = rdtsc();

 //    	elapsed += (double)(after - before);
	// }
	// printf("generating_amino_acid_chain : %lf cycles\n", elapsed / MAX_LOOP);
	// elapsed = 0;

	// free
	free(g.gene_start);
	free(g.gene_end);
	free(m.size);
	free(m.start_mut);
	free(m.end_mut);

	return 0;
}