#pragma once

inline void SetBit(unsigned int& bitField, unsigned int bit)
{
	bitField |= 0x80000000 >> bit;
}

inline void ClearBit(unsigned int& bitField, unsigned int bit)
{
	bitField &= ~(0x80000000 >> bit);
}

inline void FlipBit(unsigned int& bitfield, unsigned int bit)
{
	if ( (bitfield & (0x80000000 >> bit)) == (0x80000000 >> bit) ) 
	{
		bitfield &= ~( 0x80000000 >> bit);
	}
	else bitfield |= 0x80000000 >> bit;;
}

inline bool IsBitSet(unsigned int& bitField, unsigned int bit)
{
	if ( ((bitField << bit) & 0x80000000) == 0x80000000 ) return true;
	else return false;
}
