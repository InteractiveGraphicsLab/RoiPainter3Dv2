#pragma once

#include <vector>
#include <list>

#pragma unmanaged


//////////////////////////////////////////////////////////////////////////////////
//���̃R�[�h�͈�K�������ݐВ��ɁACGGemsJP�p��Open Source�Ƃ��Č��J�����R�[�h�Ɋ�Â�
//
//Implemented by TakashiIjiri @ Riken based on 
//"The water shed transform definitions algorithms and parallelization strategies"
//Jos B.T.M et al
//algorithm 4.1
//
//////////////////////////////////////////////////////////////////////////////////

#define TWS_WSHED       0
#define TWS_INIT       -1
#define TWS_MASK       -2
#define TWS_FICTITIOUS -3

// gradient magnitude �̃����W�� [0, 1024] �Ƃ��Ă���
#define WSD_HMIN   0
#define WSD_HMAX 255


class TWsPixelEx
{
	unsigned short m_val   ;//pixel intensity 
    int            m_label ;//pixel label INIT
    int            m_dist  ;//mask����basin/watershed pixel����̋���
	std::vector<TWsPixelEx*> m_neighbours;//reference to neighboring pixels

public:
	~TWsPixelEx(){}
    TWsPixelEx(){
		//m_value = 0 ;
		//m_dist  = 0;
		m_label = TWS_FICTITIOUS;
	}
	TWsPixelEx( const TWsPixelEx &p)
	{
		m_val   = p.m_val  ;//pixel�̋P�x�l
		m_label = p.m_label;//pixel�ɂ����x�� INIT
		m_dist  = p.m_dist ;//mask����basin/watershed pixel����̋���
		for(int i= 0;i<(int)p.m_neighbours.size(); ++i) m_neighbours.push_back( p.m_neighbours[i] );
	}

	//setter//
	inline void Set(unsigned short val){
		m_val   = std::min( (unsigned short)WSD_HMAX, std::max( val, (unsigned short)WSD_HMIN) );
		m_dist  = 0       ;
		m_label = TWS_INIT;
		m_neighbours.reserve( 26 );
    }
	inline void setLabel(int label){ m_label = label;     }
	inline void setLabelToINIT()   { m_label = TWS_INIT ; }
	inline void setLabelToMASK()   { m_label = TWS_MASK ; }
	inline void setLabelToWSHED()  { m_label = TWS_WSHED; }
	inline void setDistance(int d) { m_dist  = d    ; }

	inline void addNeighbour(TWsPixelEx *n){ m_neighbours.push_back( n ); }

	//getter
 //   inline byte getValue   () const { return m_value      ; }
    inline unsigned short getIntValue() const { return (int) m_val; } 
	inline int            getLabel   () const { return m_label; }
	inline int            getDistance() const { return m_dist ; }

	inline bool isLabelINIT()  const {return m_label == TWS_INIT ;      }
	inline bool isLabelMASK()  const {return m_label == TWS_MASK ;      } 
	inline bool isLabelWSHED() const {return m_label == TWS_WSHED;      }
	inline bool isFICTITIOUS() const {return m_label == TWS_FICTITIOUS; }

	inline std::vector<TWsPixelEx*> &getNeighbours(){ return m_neighbours; }
};




class TWsFIFO
{
	std::list<TWsPixelEx*> m_queue;
public:
    TWsFIFO(){}
    ~TWsFIFO(){}

    inline bool fifo_empty(             ) { return m_queue.empty();}
	inline void fifo_add  (TWsPixelEx *p) { m_queue.push_front(p); }

    inline TWsPixelEx *fifo_remove()
	{
		TWsPixelEx *r = m_queue.back();
		m_queue.pop_back();
		return r;
    }
};




/*
watershed algorithm
�Q�� 1) Digital image processing
     2) "The watershed transform: definitions, algorithms and parallelization strategies"

������, 2)��algorithm 4.1�ɂ�����́D
�������A36-52�ɊԈႢ��������ۂ��̂Œ��������D

���e
preprocess (1 - 13)
  pixel�̐F�ɉ����ă\�[�g
  pixel�̋ߖT���Z�b�g

flooding process (15 - 70)
  
  mask��fifo�L���[������(17 - 24)
     �lh������pixel��mask�t���O�𗧂Ă�
     ���݂�basin��watershed pixel�ɐڂ��Ă�����̂�fifo�L���[�ɓ����
  
  mask�̈����pixel�̃��x�����m�肷��(25 - 52)
     �L���[�����܂����Ǝg�����ƂŁA������basin����̗��U�����ɉ����āClabeling�̈���L���Ă���

  local minima detection(53 - 68)
     local minima��������,�elocal minima����pixel�Ɉ�т���label��ł�


*����* flat�ȗ̈悪���邽�߁A��̂悤�ȑ�ς�algorithm�ɂȂ������ۂ��ł��ˁB


25-52�ł́Amask�t���O���������e�s�N�Z����,�@������basin pixel��watershed�@pixel����̋�������
���׃����O���Ă���.

����pixel p (dist = d) �̂��ׂĂ̋ߖT q�������Ƃ��ɁAq�̏�ԂƂ��Ă��蓾����̂�
    A) basin�Awatershed�Amask�t���O�̂�����ł��Ȃ�  dist = 0       && label = INIT
	B) basin������pixel                              dist = 0 / d-1 && label = LABEL 
�@�@C) watershed pixel                               dist = 0 / d-1 && label = WATERSHED
    D) mask�t���O ���� dist = 0 �܂��L���[�ɓ����Ă��Ȃ��@�@�@�@�@�@�@ label = MASK
	E) mask�t���O ���� dist = d pixel p�Ɠ���Ɉ����Ă���            label = MASK                        

pixel p�̂��ׂĂ̋ߖT q��loop���āA
�@�@1, ���D��q���L���[�ɓ����@q.dist = d + 1�Ƃ���
  �@2, ���B��pixel����A���ׂẴ��x��������Ȃ� --> p.label = LABEL
  �@3, ���B��pixel����A���x���̎�ނ��قȂ�     --> p.label = WATERSHED
    4, ���C��pixel�����Ȃ�                       --> p.label = WATERSHED

	labels�́Awater shed��0, ����ȊO��pixel�ɂ�1����n�܂�A���̃��x�����\����
*/

inline void runWatershedAlgorithmEx(int size, TWsPixelEx **sortedPixPtr, void (*progressFunc)(double) = 0 )
{
	//run watershed algorithm////////////////////////////////////////////////////
	int curlab       = 0;
	int heightIndex1 = 0;
	int heightIndex2 = 0;
	TWsFIFO queue;

	for(int h = WSD_HMIN; h <= WSD_HMAX; ++h) //h���������ׂĂ�mask�t���O�𗧂āA����basin/watershed pixel�̗א�pixel��queue�ɓ����
	{	
		for(int pIdx = heightIndex1 ; pIdx < size; ++pIdx) 
		{
			TWsPixelEx &p = *sortedPixPtr[pIdx];
			if( p.getIntValue() != h){ heightIndex1 = pIdx; break; }

			p.setLabelToMASK();
			std::vector<TWsPixelEx*> &neighbours = p.getNeighbours();
			for(int i=0 ; i< (int) neighbours.size() ; i++) if( neighbours[i]->getLabel() >= 0 ) //basin or watershed
			{		    
				p.setDistance(1);
				queue.fifo_add(&p);
				break;
			}
		} 

		queue.fifo_add( new TWsPixelEx() );//add fictitious pixel
	    //���̃L���[�̒��g -->  mark pix pix pix ... pix ��mark��fictitious�ŁApix�́A������basin/watershed�ɗאڂ����̈�

		int curdist = 1;
	  while(true) //extend basins 
		{
			TWsPixelEx *p = queue.fifo_remove();

			if(p->isFICTITIOUS())
			{
				delete p;
				if(queue.fifo_empty()) break;
				else{
					queue.fifo_add(new TWsPixelEx());//add fictitious pixel
					++curdist;
					p = queue.fifo_remove();
				}
			}

			//neighbors�̏󋵂ɂ��p�����׃����O * �ȉ��ɐ�������
			bool hasNeighboringWsPix = false;
			std::vector<TWsPixelEx*> &neighbours = p->getNeighbours();	
			for(int i = 0; i < (int)neighbours.size(); ++i) 
			{
				TWsPixelEx &q = *neighbours[i];

				if( q.isLabelMASK() && (q.getDistance() == 0) ){
					q.setDistance( curdist+1 );
					queue.fifo_add( &q       );
				}	    

				else if( (q.getDistance() <= curdist) && q.isLabelWSHED() )  hasNeighboringWsPix = true;
				else if( (q.getDistance() <= curdist) && q.getLabel() > 0 )  //q�͊�����basin�ɓ����Ă���
				{ 
					if     ( p->isLabelMASK()                  ) p->setLabel(q.getLabel());
					else if( p->getLabel()    != q.getLabel()  ) p->setLabelToWSHED();
				}
			}
			if( p->isLabelMASK() && hasNeighboringWsPix ) p->setLabelToWSHED();//�������ˏo����wspixel������Ă��邪�ł���
		}

	    //Detect and process new minima at level h 
    for(int pIdx = heightIndex2 ; pIdx < size; pIdx++) 
    {
			TWsPixelEx &p = *sortedPixPtr[ pIdx ];
			if(p.getIntValue() != h) { heightIndex2 = pIdx; break; } // This pixel is at level h+1	

			p.setDistance( 0 ); // Reset distance to zero
			
			if(p.isLabelMASK()) { //the pixel is inside a new minimum
				curlab++;
				p.setLabel(curlab);		    
				queue.fifo_add(&p);
			    
			    //p���瓯���l(MASK���x��)��pixel��floodfill
				while(!queue.fifo_empty()) 
				{
					TWsPixelEx &q = *queue.fifo_remove();
					vector<TWsPixelEx*> &neighbours = q.getNeighbours();

					for(int i=0 ; i < (int)neighbours.size() ; ++i) if( neighbours[i]->isLabelMASK() ) 
					{
						neighbours[i]->setLabel(curlab);
						queue.fifo_add( neighbours[i] );
					}
				}
			} 
	    } 
		if( h%30==0 && progressFunc != 0) progressFunc( h / (double)WSD_HMAX);
	} 
}


inline int wsd_cmpindexEx(const void *a, const void *b)
{
	const TWsPixelEx *pa = *(const TWsPixelEx **)a;
	const TWsPixelEx *pb = *(const TWsPixelEx **)b;
	if( pb->getIntValue() < pa->getIntValue() ) return  1;
	if( pb->getIntValue() > pa->getIntValue() ) return -1;
	return 0;
}



inline void TWatershed2DEx(int W, int H, const byte *img, int *pixel_labels)
{
  //construct pixels and sort it///////////////////////////////////////////////
  const int WH = W * H;
  TWsPixelEx *pixels        = new TWsPixelEx [WH];//
  TWsPixelEx **sortedPixPtr = new TWsPixelEx*[WH];//sort��������
  for(int i = 0; i < WH; ++i) sortedPixPtr[i] = &pixels[i];

  //�P�x�l��neighbors���Z�b�g
  int idx = 0;
  for(int y = 0; y < H; ++y) 
  for(int x = 0; x < W; ++x, ++idx) 
  {
    pixels[ idx ].Set( img[idx] );
    for( int dy = -1; dy <=1 ; ++dy )
    for( int dx = -1; dx <=1 ; ++dx ) if( dx != 0 || dy != 0 )
    {
	    if( y + dy >= 0 && y + dy < H && x + dx >= 0 && x + dx < W )		
		    pixels[idx].addNeighbour( &pixels[idx + dx + dy * W] );
    }	
	}
	qsort( sortedPixPtr, WH, sizeof(TWsPixelEx*), wsd_cmpindexEx );

	//run watershed algorithm////////////////////////////////////////////////////
	runWatershedAlgorithmEx(WH, sortedPixPtr );
	for(int pIdx = 0 ; pIdx < WH ; ++pIdx) pixel_labels[pIdx] = pixels[pIdx].getLabel();

	delete[] sortedPixPtr;
	delete[] pixels;
}



inline void TWatershed3DEx( int W,  int H,  int D, const float* gMagImg, const float gMagCoef, std::vector<int> &labels, void (*progressFunc)(double) = 0 )
{
	//construct pixels and sort it///////////////////////////////////////////////
	const int size = W * H * D;
	TWsPixelEx *pixels        = new TWsPixelEx [size];//
	TWsPixelEx **sortedPixPtr = new TWsPixelEx*[size];//sort��������
	for(int i = 0; i < size; ++i) sortedPixPtr[i] = &pixels[i];

	// 9 + 8 + 9�ߖT
	int idx = 0;
	for(int z = 0; z < D; ++z) 
	for(int y = 0; y < H; ++y) 
	for(int x = 0; x < W; ++x, ++idx) 
	{
		pixels[ idx ].Set( (unsigned short)( gMagCoef * gMagImg[idx] ) );
		for( int dz = -1; dz <=1 ; ++dz )
		for( int dy = -1; dy <=1 ; ++dy )  
		for( int dx = -1; dx <=1 ; ++dx )
		{
            if( dx == 0 && dy == 0 && dz == 0 ) continue;
			
			if( z + dz >= 0 && z + dz < D && 
				y + dy >= 0 && y + dy < H && 
				x + dx >= 0 && x + dx < W ) 
				pixels[idx].addNeighbour( &pixels[idx + dx + dy * W + dz * W * H ] );
		}
	}

	qsort( sortedPixPtr, size, sizeof(TWsPixelEx*), wsd_cmpindexEx );

	//run watershed algorithm////////////////////////////////////////////////////
	runWatershedAlgorithmEx( size, sortedPixPtr, progressFunc );
	labels.resize( size);
	for(int pIdx = 0 ; pIdx < size ; ++pIdx)  labels[pIdx] = pixels[pIdx].getLabel();

	delete[] sortedPixPtr;
	delete[] pixels; 
}








//�ȉ��̊֐��͈�K@�������ǉ�
//20160826 


// computer watershed segmentation
// label��, 
void t_wsd_CalcLabelFromGMag
( 
	const int W, 
	const int H, 
	const int D, 
	const float* gMagVol, 
	const float  volCoef, 
	      vector<int> &labels
)
{
	if( W * H > 1024*1024*4 ) 
	{ 
		printf( "�f�[�^�T�C�Y���傫�����܂�\n"); 
		return;
	}

	const int stepZ = 2 * 1024 * 1024 / ( W * H) ;
	const int iterN = ( D % stepZ == 0 ) ? D / stepZ  :  D / stepZ + 1;
	printf( "watershed (%d %d %d), stepZ:%d iterN:%d \n", W,H,D, stepZ, iterN);


	vector< vector<int> > tmpLabels( iterN, vector<int>() );
	
	for( int i = 0; i < iterN; ++i)
	{
    printf( "was %d/%d \n", i, iterN );
		int z0 = i * stepZ;
		int tmpD = ( z0 + stepZ <= D ) ? stepZ : D - z0;
		TWatershed3DEx( W, H, tmpD, &gMagVol[ W * H * z0 ], volCoef, tmpLabels[i], 0 );
	}

	//assemble  multiple label ID lists  into one
	labels.resize( W*H*D, 0);
	int labelOffset = 0;
	int idxOffset   = 0;

	for( const auto &tmpLbl : tmpLabels )
	{
		int maxLabelVal = 0;
		for( int i = 0, s = (int) tmpLbl.size(); i < s; ++i) 
		{
			labels[ i + idxOffset ] = ( tmpLbl[i] == 0 ) ? 0 : tmpLbl[i] + labelOffset;
			maxLabelVal = max( maxLabelVal, tmpLbl[i] );
		}
		labelOffset += maxLabelVal;
		idxOffset   += (int) tmpLbl.size();
	}
}






// watershed�̋��E�s�N�Z���ilabels[i] == 0)���ߖT�̗̈�Ƀ}�[�W����
//�Ȃ�ׂ��F�̋߂��̈�Ƀ}�[�W����
void t_wsd_CollapseWsdPixels3D(
	const int W, 
	const int H, 
	const int D, 
	const short* vol, 
	vector<int> &labels)
{
	const int WH = W*H, WHD = W*H*D;

	byte *visFlg = new byte[WHD];

	while( true )
	{
		memset( visFlg, 0, sizeof(byte) * WHD);

		int idx = 0;
		for( int z = 0; z < D; ++z        )	
		for( int y = 0; y < H; ++y        )	
		for( int x = 0; x < W; ++x, ++idx ) if( labels[ idx ] <= 0 )
		{	
			//���A�E�A��A��
			float colDiff = FLT_MAX;

			for( int c = 0; c<6; ++c)
			{
				int neiIdx = 0;
				if     ( c == 0 && x >   0 ) neiIdx = idx - 1 ;//��
				else if( c == 1 && x < W-1 ) neiIdx = idx + 1 ;//�E
				else if( c == 2 && y >   0 ) neiIdx = idx - W ;//��
				else if( c == 3 && y < H-1 ) neiIdx = idx + W ;//��
				else if( c == 4 && z >   0 ) neiIdx = idx - WH;
				else if( c == 5 && z < D-1 ) neiIdx = idx + WH;
				else continue;

				if( labels[ neiIdx ] <= 0 || visFlg[ neiIdx ] != 0) continue;
				
				float d = (vol[idx] - vol[neiIdx])*(vol[idx] - (float)vol[neiIdx]);

				if( d < colDiff )
				{ 
					colDiff = d; 
					labels[idx] = labels[ neiIdx ];
					visFlg[idx] = 1;
				}
			}
		}

		bool boundExist = false;
		for( int i=0; i < WHD; ++i) if( labels[i] <= 0) 
		{
			boundExist = true;
			break;
		}
		if( !boundExist ) break;

		//fprintf( stderr, "t_wsd_CollapseWsdPixels3D loop end \n");
	}

	delete[] visFlg;
}




//�@1 voxel region��watershed���E�̈�ɂ���i���������̂��߁j
// label�̐U��Ȃ����̕K�v��������
void t_wsd_RemoveOneVoxWsdLabel( vector<int> &v_labels)
{

	//max label 
	int maxLabel = 0;
	for( const auto& vL: v_labels) maxLabel = max( maxLabel, vL);
	
	vector<int> label_voxN( maxLabel + 1, 0);
	for( const auto& vL: v_labels) ++label_voxN[ vL ];
	
	//1 voxel �̈�����E�v�f��
	for(       auto& vL: v_labels) if( label_voxN[ vL ] == 1 )
	{
		label_voxN[vL] = -1;
		vL = 0;
	}

	//label��A���ɐU�蒼��
	int offset = 0;
	vector<int> newLabel( label_voxN.size() );
	for( int i = 0, s = (int) label_voxN.size(); i < s; ++i)
	{
		if( label_voxN[i] == -1 ) offset++;
		else newLabel[i] = i-offset;
	}

	for( auto& vL: v_labels) vL = newLabel[ vL ];
}


#pragma managed
