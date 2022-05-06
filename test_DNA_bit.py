import pytest
import DNA_bit
import array
import numpy as np
import moduleDNA as m

###### Verify if C extension in Python is working #####

def test_DNA_convert_to_binary():
  	# --- Test all valid letters
  	# A 00 T 11 C 10 G 01

	res = DNA_bit.convert_to_binary("AATTG",5)
	assert res == [0,0,0,0,1,1,1,1,0,1]
	res = DNA_bit.convert_to_binary("ATCGATCGATCG", 12)
	assert res == [0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1]
    # Give a different size than the char sequence.
	res = DNA_bit.convert_to_binary("ATCG", 3)
	assert res == [0,0,1 ,1,1,0]

	res = DNA_bit.convert_to_binary("ATCG", 5)
	assert res == [0,0,1 ,1,1,0,0,1,0,0]

	res = DNA_bit.convert_to_binary("ATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCG", 16)
	assert res == [0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1]	

	res = DNA_bit.convert_to_binary("ATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCGATCG", 32)
	assert res == [0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1,0,0,1 ,1,1,0,0,1]	


#def test_DNA_module_read_file():
#	res = m.read_file("fastas/test.fasta")
#	assert res == "AATTG"

def test_DNA_generating_mRNA():
  	# --- Test all valid letters
  	# A 00 U 11 C 10 G 01	
	res = DNA_bit.generating_mRNA(bytearray([0,1,0,0,1,1,0,0,0,1,1,0]))
	assert res == b"GAUAGC"

	res = DNA_bit.generating_mRNA(bytearray([0,0,1,1,0,1,1,0,0,1,1,1,0,1,0,1,0,1,1,1,0,0,0,1]))
	assert res == b"AUGCGUGGGUAG"	

	res = DNA_bit.generating_mRNA(bytearray([0,0,1,1,0,1,1,0,0,1,1,1,0,1,0,1,0,1,1,1,0,0,0,1,0,0,1,1,0,1,1,0]))
	assert res == b"AUGCGUGGGUAGAUGC"	

	res = DNA_bit.generating_mRNA(bytearray([0,0,1,1,0,1,1,0,0,1,1,1,0,1,0,1,0,1,1,1,0,0,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]))
	assert res == b"AUGCGUGGGUAGAUGCAAAAAAAAAAAAAAAA"	

	res = DNA_bit.generating_mRNA(bytearray([0,0,1,1,0,1,1,0,0,1,1,1,0,1,0,1,0,1,1,1,0,0,0,1,0,0,1,1,0,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,0,1,0,1,0,1,1,1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0]))
	assert res == b"AUGCGUGGGUAGAUGCAAAAAAAAAAAAAAAAGUGGGUAGAAAAAAAAAAAAAAAAAAAAAAAA"	




def test_DNA_detecting_genes():
	a = DNA_bit.detecting_genes(bytearray([0,0,1,1,0,1,0,0,1,1,0,1,1,1,0,0,0,0]))
	assert a == [[6,17]]

	# Test if the algorithm is OK in a basic case: xxxAUGxxxxUAAxxx
  	#                                             AGC AUG AGGGCC UAA CGU
  	# The algorithm should detect one gene from the start codon to the stop codon
  
	res = DNA_bit.detecting_genes(bytearray(DNA_bit.convert_to_binary("AGCATGAGGGCCTAACGT", 21)))
	assert res == [[6,29]]

  	# Test if the algorithm is OK in a multiple "start" case: xxxxAUGxxxxAUGxxxUAAxxx
  	#                                                        AGC AUG AGGGCC AUG CGAACG UAA CGU
  	# The algorithm should detect one gene from the 2nd start codon to the stop codon
	
	res =  DNA_bit.detecting_genes(bytearray(DNA_bit.convert_to_binary("AGCATGAGGGCCATGCGAACGTAACGT", 27)))
	assert res == [[24,47]]

  	# Test if the algorithm is OK in a multiple "stop" case: xxxxAUGxxxxUAAxxxUAAxxx
  	#                                                         AGC AUG CACGCG UAA GCACTG UAA CGU
  	# The algorithm should detect one gene from the start codon to the first stop codon
	res =  DNA_bit.detecting_genes(bytearray(DNA_bit.convert_to_binary("AGCATGCACGCGTAAGCACTGTAACGT", 27)))
	assert res == [[6,29]]

	# Test if the algorithm is OK in a non presence of "start/stop" case: xxxxxxxxx
    #                                                                   CGCCGCGCCGCGGGCG
    # The algorithm should not detect any genes
	res =  DNA_bit.detecting_genes(bytearray(DNA_bit.convert_to_binary("CGCCGCGCCGCGGGCG", 16)))
	assert len(res) == 0

	# Test if the algorithm is OK in a multiple gene case: xxxxAUGxxxxUAGxxxAUGxxxUAAxxx
    #                                                    AGC AUG GCGCAC UAG CGCCCG AUG CUGGGG UAA CGU
  	# The algorithm should detect two genes
	res =  DNA_bit.detecting_genes(bytearray(DNA_bit.convert_to_binary("AGCATGGCGCACTAGCGCCCGATGCTGGGGTAACGT", 36)))
	assert res == [[6,29],[42,65]]


def test_DNA_generating_amino_acid_chain():
	aa = "AATTGGCCA"

	bin_dna_seq_array = DNA_bit.convert_to_binary(aa,len(aa))

	bin_dna_seq = bytearray( [0, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 0, 0, 0])
	bin_dna_seq_array = bytearray( bin_dna_seq_array)

	res1 = DNA_bit.generating_amino_acid_chain(bin_dna_seq)
	res2 = DNA_bit.generating_amino_acid_chain(bin_dna_seq_array)

	assert b"NWP" == res1
	assert b"NWP" == res2

def test_DNA_detecting_mutations():
	res = DNA_bit.detecting_mutations(bytearray( [0,1,0,1,1,0,0,0,0,0,0,0]))
	assert res[0] == [5,0,5]

	res = DNA_bit.detecting_mutations(bytearray( [0,1,0,0,1,1,0,0,0,1,1,0]))
	assert res[0] == [1,0,1]
	assert res[1] == [3,8,11]

	res = DNA_bit.detecting_mutations(bytearray( [0,0,0,1,0,0,0,0,0,0,0,1]))
	assert res[0] == [1,2,3]
	assert res[1] == [1,10,11]

def test_DNA_calculating_matching_score():
	size = 20
	ar1 = bytearray( [0 for i in range(size)])
	ar2 = bytearray( [1 for i in range(size)])
	for i in range(size):
		ar1[i] = 1
		assert (i+1)*100/size == DNA_bit.calculating_matching_score(ar1, ar2)
	# Warning here, (i+1)*100/size might generate a floating number with low precision error, which might result in a false assert.
