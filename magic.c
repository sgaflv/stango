#include "magic.h"




int RPopBits[64] = {
  12, 11, 11, 11, 11, 11, 11, 12,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  11, 10, 10, 10, 10, 10, 10, 11,
  12, 11, 11, 11, 11, 11, 11, 12
};
 
int BPopBits[64] = {
  6, 5, 5, 5, 5, 5, 5, 6,
  5, 5, 5, 5, 5, 5, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 9, 9, 7, 5, 5,
  5, 5, 7, 7, 7, 7, 5, 5,
  5, 5, 5, 5, 5, 5, 5, 5,
  6, 5, 5, 5, 5, 5, 5, 6
};

int RShiftBits[64];
int BShiftBits[64];

U64 *RAttack[64]; /* 64 pointers to rook magic number arrays */
U64 *BAttack[64]; /* 64 pointers to bishop magic number arrays */

void initPopBits() {
	int i;
	
	for (i=0;i<64;i++) {
		RShiftBits[i]=64-RPopBits[i];
		BShiftBits[i]=64-BPopBits[i];
	}
}

/* computes rook attack bits for a given field of obstacles */
static U64 compute_rook_attack_bits(int x, int y, U64 field) {
	int dir1, dir2, dir3, dir4, k;
	U64 attack=0;
	dir1=dir2=dir3=dir4=1;
	for (k=1;k<8;k++) {
		if (dir1) add_bit(&attack, x+k, y);
		if (dir2) add_bit(&attack, x-k, y);
		if (dir3) add_bit(&attack, x, y+k);
		if (dir4) add_bit(&attack, x, y-k);
		dir1&=!get_bit(&field, x+k, y);
		dir2&=!get_bit(&field, x-k, y);
		dir3&=!get_bit(&field, x, y+k);
		dir4&=!get_bit(&field, x, y-k);
	}
	return attack;
}

/* computes bishop attack bits for a given field of obstacles */
static U64 compute_bishop_attack_bits(int x, int y, U64 field) {
	int dir1, dir2, dir3, dir4, k;
	U64 attack=0;
	dir1=dir2=dir3=dir4=1;
	for (k=1;k<8;k++) {
		if (dir1) add_bit(&attack, x+k, y+k);
		if (dir2) add_bit(&attack, x+k, y-k);
		if (dir3) add_bit(&attack, x-k, y+k);
		if (dir4) add_bit(&attack, x-k, y-k);
		dir1&=!get_bit(&field, x+k, y+k);
		dir2&=!get_bit(&field, x+k, y-k);
		dir3&=!get_bit(&field, x-k, y+k);
		dir4&=!get_bit(&field, x-k, y-k);
	}
	return attack;
}

void initMagic() {
	int i, j, idx, x, y, rbits, rp, bbits, bp;
	U64 field, index, attack;
	U64 k, tmp;
	

	/* generate all possible inner attacks for rooks, */
	for (y=0;y<8;y++) {
		for (x=0;x<8;x++) {
			idx=x+8*y;
			
			rbits=RPopBits[idx];
			rp=1<<rbits; /* number of magic numbers for this square: 2 pow bits */

			RAttack[idx]=malloc(rp*(sizeof(U64)));
			
			/* process rooks */
			for (i=0;i<rp;i+=1) {
					
				field=0;
				k=iRook[idx];
				j=i;
				
				while(k) {
					tmp = k&(0-k);
					if (j&1) field|=tmp;
					j=j>>1;
					k^=tmp;
				}
				
				field&=iRook[idx];
				index=field*RMagic[idx];
				index>>=RShiftBits[idx];
				
				attack=compute_rook_attack_bits(x, y, field);
				
				RAttack[idx][index]=attack;
			}
			
			
			bbits=RPopBits[idx];
			bp=1<<bbits; /* number of magic numbers for this square: 2 pow bits */
			
			BAttack[idx]=malloc(bp*(sizeof(U64)));
			memset(BAttack[idx], 0, bp*(sizeof(U64)));
			
			/* now bishops */
			for (i=0;i<bp;i+=1) {
				
				j=i;
					
				field=0;
				k=iBishop[idx];
				j=i;
				
				while(k) {
					tmp = k&(0-k);
					if (j&1) field|=tmp;
					j=j>>1;
					k^=tmp;
				}
				
				field&=iBishop[idx];
				
				index=field*BMagic[idx];
				index>>=BShiftBits[idx];
				
				attack=compute_bishop_attack_bits(x, y, field);
				
				/*
				if (BAttack[idx][index]&&
						BAttack[idx][index]!=attack) {
						printf("\n\nredefining!\n");
						output_bitboard(BAttack[idx][index]);
						output_bitboard(attack);
					}
				*/
				BAttack[idx][index]=attack;
			}
		}
	}
}

/* checks if a magic number is good */
static bool checkRookMagic(int idx, U64 magic) {
	int x, y, i, j, k;
	int shift = RShiftBits[idx];
	U64 attack, field, index;
	int pop = RPopBits[idx];
	U64 mcheck[1<<pop];
	memset(mcheck, 0, sizeof mcheck);
	
	x=idx&7;
	y=idx>>3;
	
	/* process rooks */
	for (i=0;i<256;i+=1) {
		for (j=0;j<256;j+=1) {
			
			field=0;
			for (k=0;k<8;k++) {
				if ((j>>k)&1) add_bit(&field, (x+k)%8, y);
				if ((i>>k)&1) add_bit(&field, x, (y+k)%8);
			}
			
			field&=iRook[idx];
			index=field*magic;
			index>>=shift;
			
			attack=compute_rook_attack_bits(x, y, field);
			if (mcheck[index]==0)
				mcheck[index]=attack;
			else if (mcheck[index]!=attack) {
//				printf("given magic fails\n");
				return false;
			}
		}
	}
	return true;
}


/* checks if a magic number is good */
static bool checkBishopMagic(int idx, U64 magic) {
	int x, y, i, j, k;
	int shift = BShiftBits[idx];
	U64 attack, field, index;
	int pop = BPopBits[idx];
	U64 mcheck[1<<pop];
	memset(mcheck, 0, sizeof mcheck);
	
	x=idx&7;
	y=idx>>3;
	
	/* process rooks */
	for (i=0;i<256;i+=1) {
		for (j=0;j<256;j+=1) {
			
			field=0;
			for (k=0;k<8;k++) {
				if ((j>>k)&1) add_bit(&field, (x+k)%8, (y+k)%8);
				if ((i>>k)&1) add_bit(&field, (x+k)%8, (y+8-k)%8);
			}
			
			field&=iBishop[idx];
			index=field*magic;
			index>>=shift;
			
			attack=compute_bishop_attack_bits(x, y, field);
			if (mcheck[index]==0)
				mcheck[index]=attack;
			else if (mcheck[index]!=attack) {
//				printf("given magic fails\n");
				return false;
			}
		}
	}
	return true;
}


 
static U64 random_fewbits() {
  return random_U64() & random_U64() & random_U64();
}


static U64 generateRookMagic(int idx) {
	int i;
	U64 magic;
	
	for (i=0;i<1000000;i++) {
		magic=random_fewbits();
		if (checkRookMagic(idx, magic)) {
			printf("\t0x%llXULL", magic);
			return magic;
		}
	}
	
	printf("failed to find any magic!\n");
	return 0;
}

static U64 generateBishopMagic(int idx) {
	int i;
	U64 magic;
	
	for (i=0;i<1000000;i++) {
		magic=random_fewbits();
		if (checkBishopMagic(idx, magic)) {
			printf("\t0x%llXULL", magic);
			return magic;
		}
	}
	
	printf("failed to find any magic!\n");
	return 0;
}

void generateAllRookMagic() {
	int i;
	printf("const U64 RMagic[64] = {\n");
	for (i=0;i<64;i++) {
		generateRookMagic(i);
		if (i==63) 
			printf("\n};\n\n");
		else
			printf(",\n");
		fflush(stdout);
	}
}

void generateAllBishopMagic() {
	int i;
	printf("const U64 BMagic[64] = {\n");
	for (i=0;i<64;i++) {
		generateBishopMagic(i);
		if (i==63) 
			printf("\n};\n\n");
		else
			printf(",\n");
		fflush(stdout);
	}
}

const U64 RMagic[64] = {
	0xA8002C000108020ULL,
	0x4440200140003000ULL,
	0x8080200010011880ULL,
	0x380180080141000ULL,
	0x1A00060008211044ULL,
	0x410001000A0C0008ULL,
	0x9500060004008100ULL,
	0x100024284A20700ULL,
	0x802140008000ULL,
	0x80C01002A00840ULL,
	0x402004282011020ULL,
	0x9862000820420050ULL,
	0x1001448011100ULL,
	0x6432800200800400ULL,
	0x40100010002000CULL,
	0x2800D0010C080ULL,
	0x90C0008000803042ULL,
	0x4010004000200041ULL,
	0x3010010200040ULL,
	0xA40828028001000ULL,
	0x123010008000430ULL,
	0x24008004020080ULL,
	0x60040001104802ULL,
	0x582200028400D1ULL,
	0x4000802080044000ULL,
	0x408208200420308ULL,
	0x610038080102000ULL,
	0x3601000900100020ULL,
	0x80080040180ULL,
	0xC2020080040080ULL,
	0x80084400100102ULL,
	0x4022408200014401ULL,
	0x40052040800082ULL,
	0xB08200280804000ULL,
	0x8A80A008801000ULL,
	0x4000480080801000ULL,
	0x911808800801401ULL,
	0x822A003002001894ULL,
	0x401068091400108AULL,
	0x4A10A00004CULL,
	0x2000800640008024ULL,
	0x1486408102020020ULL,
	0x100A000D50041ULL,
	0x810050020B0020ULL,
	0x204000800808004ULL,
	0x20048100A000CULL,
	0x112000831020004ULL,
	0x9000040810002ULL,
	0x440490200208200ULL,
	0x8910401000200040ULL,
	0x6404200050008480ULL,
	0x4B824A2010010100ULL,
	0x4080801810C0080ULL,
	0x400802A0080ULL,
	0x8224080110026400ULL,
	0x40002C4104088200ULL,
	0x1002100104A0282ULL,
	0x1208400811048021ULL,
	0x3201014A40D02001ULL,
	0x5100019200501ULL,
	0x101000208001005ULL,
	0x2008450080702ULL,
	0x1002080301D00CULL,
	0x410201CE5C030092ULL
};

const U64 BMagic[64] = {
	0x40210414004040ULL,
	0x2290100115012200ULL,
	0xA240400A6004201ULL,
	0x80A0420800480ULL,
	0x4022021000000061ULL,
	0x31012010200000ULL,
	0x4404421051080068ULL,
	0x1040882015000ULL,
	0x8048C01206021210ULL,
	0x222091024088820ULL,
	0x4328110102020200ULL,
	0x901CC41052000D0ULL,
	0xA828C20210000200ULL,
	0x308419004A004E0ULL,
	0x4000840404860881ULL,
	0x800008424020680ULL,
	0x28100040100204A1ULL,
	0x82001002080510ULL,
	0x9008103000204010ULL,
	0x141820040C00B000ULL,
	0x81010090402022ULL,
	0x14400480602000ULL,
	0x8A008048443C00ULL,
	0x280202060220ULL,
	0x3520100860841100ULL,
	0x9810083C02080100ULL,
	0x41003000620C0140ULL,
	0x6100400104010A0ULL,
	0x20840000802008ULL,
	0x40050A010900A080ULL,
	0x818404001041602ULL,
	0x8040604006010400ULL,
	0x1028044001041800ULL,
	0x80B00828108200ULL,
	0xC000280C04080220ULL,
	0x3010020080880081ULL,
	0x10004C0400004100ULL,
	0x3010020200002080ULL,
	0x202304019004020AULL,
	0x4208A0000E110ULL,
	0x108018410006000ULL,
	0x202210120440800ULL,
	0x100850C828001000ULL,
	0x1401024204800800ULL,
	0x41028800402ULL,
	0x20642300480600ULL,
	0x20410200800202ULL,
	0xCA02480845000080ULL,
	0x140C404A0080410ULL,
	0x2180A40108884441ULL,
	0x4410420104980302ULL,
	0x1108040046080000ULL,
	0x8141029012020008ULL,
	0x894081818082800ULL,
	0x40020404628000ULL,
	0x804100C010C2122ULL,
	0x8168210510101200ULL,
	0x1088148121080ULL,
	0x204010100C11010ULL,
	0x1814102013841400ULL,
	0xC00010020602ULL,
	0x1045220C040820ULL,
	0x12400808070840ULL,
	0x2004012A040132ULL
};
