/**
 ** Copyright (c) 2011 Illumina, Inc.
 **
 ** 
 ** This software is covered by the "Illumina Non-Commercial Use Software
 ** and Source Code License Agreement" and any user of this software or
 ** source file is bound by the terms therein (see accompanying file
 ** Illumina_Non-Commercial_Use_Software_and_Source_Code_License_Agreement.pdf)
 **
 ** This file is part of the BEETL software package.
 **
 ** Citation: Markus J. Bauer, Anthony J. Cox and Giovanna Rosone
 ** Lightweight BWT Construction for Very Large String Collections. 
 ** Proceedings of CPM 2011, pp.219-231
 **
 **/

#include "TransposeFasta.h"
#include "Tools.h"
#include <assert.h>


using std::cout;
using std::cerr;
using std::endl;

TransposeFasta::TransposeFasta()
{

}


TransposeFasta::~TransposeFasta()
{

}


bool TransposeFasta::convert( const string& input,const string& output )
{
	//TO DO
	lengthRead = CYCLENUM;
	//The distribution of characters is useful
	//for alpha[256] -->Corresponding between the alphabet, the piles and tableOcc
	//and to know sizeAlpha
	//We supposed that the symbols in the input file are the following
	freq[int(TERMINATE_CHAR)]=1;
	freq[int('A')]=1;
	freq[int('C')]=1;
	freq[int('G')]=1;
	freq[int('N')]=1;
	freq[int('T')]=1;

    FILE* ifile;
	ifile = fopen(input.c_str(), "rb");
    
    if( ifile == NULL ) { cerr << "TrasposeFasta: could not open file " << input << " !" << endl; }

    // create output files
    for(dataTypelenSeq i=0;i<CYCLENUM;i++ )
    {
        std::stringstream fn;
        fn << output <<  i << ".txt";
        outputFiles_[i] = fopen( fn.str().c_str(),"w" );
    }


    // looping through the input file, add the characters to the buffer, print buffer when it's full
//    unsigned int num_read = 0;
    unsigned int num_write = 0;    
    unsigned int charsBuffered = 0;

    	//******************************buf[CYCLENUM+1];  ********* is CYCLENUM right?
	char buf[CYCLENUM+1];
	lengthTexts = 0;


    for(dataTypelenSeq i=0;i<CYCLENUM;i++ )
    {
        buf[i] = '\0';
    }

	nSeq = 0;
//    num_read = fread(buf,sizeof(uchar),CYCLENUM,ifile);
    fgets ( buf,1024, ifile );
    while( !feof(ifile) )
    {
        //cerr << "current line : " << buf << endl;
        
        if( charsBuffered == BUFFERSIZE )
        {
            // write buffers to the files, clear buffers
            for(dataTypelenSeq i=0;i<CYCLENUM;i++ )
            {
                //cerr << "writing to " << i << " : " << buf_[i] << endl;
                num_write = fwrite ( buf_[i],sizeof(char),charsBuffered,outputFiles_[i] );
				lengthTexts += num_write;
            }
            checkIfEqual( num_write,charsBuffered ); // we should always read/write the same number of characters


            charsBuffered=0;
        }


        // process the input
        if( buf[0] != '>' ) 
        {
            // add the characters
            for(dataTypelenSeq i=0;i<CYCLENUM;i++ )
            {
                buf_[i][charsBuffered] = buf[i];
            }
            
            // increase the counter of chars buffered
            charsBuffered++;            
        }
		else 
			nSeq++;

        //num_read = fread(buf,sizeof(uchar),CYCLENUM,ifile);        
        fgets ( buf, 1024, ifile );
    }

    // write the rest
    for(dataTypelenSeq i=0;i<CYCLENUM;i++ )
    {
        num_write = fwrite ( buf_[i],sizeof(uchar),charsBuffered,outputFiles_[i] );
		lengthTexts += num_write;
    }
    checkIfEqual( num_write,charsBuffered );



    // closing all the output file streams
    for(dataTypelenSeq i=0;i<CYCLENUM;i++ )
    {
        fclose( outputFiles_[i] );
    }

	std::cout << "Number of sequences reading/writing: " << nSeq << "\n";
	std::cout << "Number of characters reading/writing: " << lengthTexts << "\n";

    return true;
}
