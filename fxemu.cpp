/**********************************************************************************
  Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.

  (c) Copyright 1996 - 2002  Gary Henderson (gary.henderson@ntlworld.com) and
                             Jerremy Koot (jkoot@snes9x.com)

  (c) Copyright 2002 - 2004  Matthew Kendora

  (c) Copyright 2002 - 2005  Peter Bortas (peter@bortas.org)

  (c) Copyright 2004 - 2005  Joel Yliluoma (http://iki.fi/bisqwit/)

  (c) Copyright 2001 - 2006  John Weidman (jweidman@slip.net)

  (c) Copyright 2002 - 2006  Brad Jorsch (anomie@users.sourceforge.net),
                             funkyass (funkyass@spam.shaw.ca),
                             Kris Bleakley (codeviolation@hotmail.com),
                             Nach (n-a-c-h@users.sourceforge.net), and
                             zones (kasumitokoduck@yahoo.com)

  BS-X C emulator code
  (c) Copyright 2005 - 2006  Dreamer Nom,
                             zones

  C4 x86 assembler and some C emulation code
  (c) Copyright 2000 - 2003  _Demo_ (_demo_@zsnes.com),
                             Nach,
                             zsKnight (zsknight@zsnes.com)

  C4 C++ code
  (c) Copyright 2003 - 2006  Brad Jorsch,
                             Nach

  DSP-1 emulator code
  (c) Copyright 1998 - 2006  _Demo_,
                             Andreas Naive (andreasnaive@gmail.com)
                             Gary Henderson,
                             Ivar (ivar@snes9x.com),
                             John Weidman,
                             Kris Bleakley,
                             Matthew Kendora,
                             Nach,
                             neviksti (neviksti@hotmail.com)

  DSP-2 emulator code
  (c) Copyright 2003         John Weidman,
                             Kris Bleakley,
                             Lord Nightmare (lord_nightmare@users.sourceforge.net),
                             Matthew Kendora,
                             neviksti


  DSP-3 emulator code
  (c) Copyright 2003 - 2006  John Weidman,
                             Kris Bleakley,
                             Lancer,
                             z80 gaiden

  DSP-4 emulator code
  (c) Copyright 2004 - 2006  Dreamer Nom,
                             John Weidman,
                             Kris Bleakley,
                             Nach,
                             z80 gaiden

  OBC1 emulator code
  (c) Copyright 2001 - 2004  zsKnight,
                             pagefault (pagefault@zsnes.com),
                             Kris Bleakley,
                             Ported from x86 assembler to C by sanmaiwashi

  SPC7110 and RTC C++ emulator code
  (c) Copyright 2002         Matthew Kendora with research by
                             zsKnight,
                             John Weidman,
                             Dark Force

  S-DD1 C emulator code
  (c) Copyright 2003         Brad Jorsch with research by
                             Andreas Naive,
                             John Weidman

  S-RTC C emulator code
  (c) Copyright 2001-2006    byuu,
                             John Weidman

  ST010 C++ emulator code
  (c) Copyright 2003         Feather,
                             John Weidman,
                             Kris Bleakley,
                             Matthew Kendora

  Super FX x86 assembler emulator code
  (c) Copyright 1998 - 2003  _Demo_,
                             pagefault,
                             zsKnight,

  Super FX C emulator code
  (c) Copyright 1997 - 1999  Ivar,
                             Gary Henderson,
                             John Weidman

  Sound DSP emulator code is derived from SNEeSe and OpenSPC:
  (c) Copyright 1998 - 2003  Brad Martin
  (c) Copyright 1998 - 2006  Charles Bilyue'

  SH assembler code partly based on x86 assembler code
  (c) Copyright 2002 - 2004  Marcus Comstedt (marcus@mc.pp.se)

  2xSaI filter
  (c) Copyright 1999 - 2001  Derek Liauw Kie Fa

  HQ2x filter
  (c) Copyright 2003         Maxim Stepin (maxim@hiend3d.com)

  Specific ports contains the works of other authors. See headers in
  individual files.

  Snes9x homepage: http://www.snes9x.com

  Permission to use, copy, modify and/or distribute Snes9x in both binary
  and source form, for non-commercial purposes, is hereby granted without 
  fee, providing that this license information and copyright notice appear 
  with all copies and any derived work.

  This software is provided 'as-is', without any express or implied
  warranty. In no event shall the authors be held liable for any damages
  arising from the use of this software or it's derivatives.

  Snes9x is freeware for PERSONAL USE only. Commercial users should
  seek permission of the copyright holders first. Commercial use includes,
  but is not limited to, charging money for Snes9x or software derived from
  Snes9x, including Snes9x or derivatives in commercial game bundles, and/or
  using Snes9x as a promotion for your commercial product.

  The copyright holders request that bug fixes and improvements to the code
  should be forwarded to them so everyone can benefit from the modifications
  in future versions.

  Super NES and Super Nintendo Entertainment System are trademarks of
  Nintendo Co., Limited and its subsidiary companies.
**********************************************************************************/

#include "memmap.h"
#include "fxemu.h"
#include "fxinst.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/* The FxChip Emulator's internal variables */
struct FxRegs_s GSU = FxRegs_s_null;
extern struct FxInit_s SuperFX;

uint32 (**fx_ppfFunctionTable)(uint32) = 0;
void (**fx_ppfPlotTable)() = 0;
void (**fx_ppfOpcodeTable)() = 0;

/*#if 0
void fx_setCache()
{
    uint32 c;
    GSU.bCacheActive = TRUE;
    GSU.pvRegisters[0x3e] &= 0xf0;
    c = (uint32)GSU.pvRegisters[0x3e];
    c |= ((uint32)GSU.pvRegisters[0x3f])<<8;
    if(c == GSU.vCacheBaseReg)
	return;
    GSU.vCacheBaseReg = c;
    GSU.vCacheFlags = 0;
    if(c < (0x10000-512))
    {
	uint8 const* t = &ROM(c);
	memcpy(GSU.pvCache,t,512);
    }
    else
    {
	uint8 const* t1;
	uint8 const* t2;
	uint32 i = 0x10000 - c;
	t1 = &ROM(c);
	t2 = &ROM(0);
	memcpy(GSU.pvCache,t1,i);
	memcpy(&GSU.pvCache[i],t2,512-i);
    }
}
#endif*/

/*void FxCacheWriteAccess(uint16 vAddress)
{
#if 0
    if(!GSU.bCacheActive)
    {
	uint8 v = GSU.pvCache[GSU.pvCache[vAddress&0x1ff];
	fx_setCache();
	GSU.pvCache[GSU.pvCache[vAddress&0x1ff] = v;
    }
#endif
    if((vAddress & 0x00f) == 0x00f)
	GSU.vCacheFlags |= 1 << ((vAddress&0x1f0) >> 4);
}*/

/*void FxFlushCache()
{
    GSU.vCacheFlags = 0;
    GSU.vCacheBaseReg = 0;
    GSU.bCacheActive = FALSE;
	//    GSU.vPipe = 0x1;
}*/

/*static void fx_backupCache()
{
#if 0
    uint32 i;
    uint32 v = GSU.vCacheFlags;
    uint32 c = USEX16(GSU.vCacheBaseReg);
    if(v)
	for(i=0; i<32; i++)
	{
	    if(v&1)
	    {
		if(c < (0x10000-16))
		{
		    uint8 * t = &GSU.pvPrgBank[c];
		    memcpy(&GSU.avCacheBackup[i<<4],t,16);
		    memcpy(t,&GSU.pvCache[i<<4],16);
		}
		else
		{
		    uint8 * t1;
		    uint8 * t2;
		    uint32 a = 0x10000 - c;
		    t1 = &GSU.pvPrgBank[c];
		    t2 = &GSU.pvPrgBank[0];
		    memcpy(&GSU.avCacheBackup[i<<4],t1,a);
		    memcpy(t1,&GSU.pvCache[i<<4],a);
		    memcpy(&GSU.avCacheBackup[(i<<4)+a],t2,16-a);
		    memcpy(t2,&GSU.pvCache[(i<<4)+a],16-a);
		}		
	    }
	    c = USEX16(c+16);
	    v >>= 1;
	}
#endif
}*/

/*static void fx_restoreCache()
{
#if 0
    uint32 i;
    uint32 v = GSU.vCacheFlags;
    uint32 c = USEX16(GSU.vCacheBaseReg);
    if(v)
	for(i=0; i<32; i++)
	{
	    if(v&1)
	    {
		if(c < (0x10000-16))
		{
		    uint8 * t = &GSU.pvPrgBank[c];
		    memcpy(t,&GSU.avCacheBackup[i<<4],16);
		    memcpy(&GSU.pvCache[i<<4],t,16);
		}
		else
		{
		    uint8 * t1;
		    uint8 * t2;
		    uint32 a = 0x10000 - c;
		    t1 = &GSU.pvPrgBank[c];
		    t2 = &GSU.pvPrgBank[0];
		    memcpy(t1,&GSU.avCacheBackup[i<<4],a);
		    memcpy(&GSU.pvCache[i<<4],t1,a);
		    memcpy(t2,&GSU.avCacheBackup[(i<<4)+a],16-a);
		    memcpy(&GSU.pvCache[(i<<4)+a],t2,16-a);
		}		
	    }
	    c = USEX16(c+16);
	    v >>= 1;
	}
#endif
}*/

/*void fx_flushCache()
{
	//fx_restoreCache();
    GSU.vCacheFlags = 0;
    GSU.bCacheActive = FALSE;
}*/

/*void fx_updateRamBank(uint8 Byte)
{
	// Update BankReg and Bank pointer
    GSU.vRamBankReg = (uint32)Byte & (FX_RAM_BANKS-1);
    GSU.pvRamBank = GSU.apvRamBank[Byte & 0x3];
}*/

/*static void fx_readRegisterSpaceForCheck(void)
{
   R15 = GSU.pvRegisters[30];
   R15 |= ((uint32_t) GSU.pvRegisters[31]) << 8;
}*/

static inline void fx_readRegisterSpaceForUse()
{
    int i;
    uint8 *p = GSU.pvRegisters;
    static uint32 avHeight[] = { 128, 160, 192, 256 };
    static uint32 avMult[] = { 16, 32, 32, 64 };

    GSU.vErrorCode = 0;

    /* Update R0-R14 */
    for(i = 0; i < 15; i++)
    {
		GSU.avReg[i] = *p++;
		GSU.avReg[i] += ((uint32)(*p++)) << 8;
    }

    /* Update other registers */
    p = GSU.pvRegisters;
    GSU.vStatusReg = (uint32)p[GSU_SFR];
    GSU.vStatusReg |= ((uint32)p[GSU_SFR+1]) << 8;
    GSU.vPrgBankReg = (uint32)p[GSU_PBR];
    GSU.vRomBankReg = (uint32)p[GSU_ROMBR];
    GSU.vRamBankReg = ((uint32)p[GSU_RAMBR]) & (FX_RAM_BANKS-1);
    GSU.vCacheBaseReg = (uint32)p[GSU_CBR];
    GSU.vCacheBaseReg |= ((uint32)p[GSU_CBR+1]) << 8;

    /* Update status register variables */
    GSU.vZero = !(GSU.vStatusReg & FLG_Z);
    GSU.vSign = (GSU.vStatusReg & FLG_S) << 12;
    GSU.vOverflow = (GSU.vStatusReg & FLG_OV) << 16;
    GSU.vCarry = (GSU.vStatusReg & FLG_CY) >> 2;
    
    /* Set bank pointers */
    GSU.pvRamBank = GSU.apvRamBank[GSU.vRamBankReg & 0x3];
    GSU.pvRomBank = GSU.apvRomBank[GSU.vRomBankReg];
    GSU.pvPrgBank = GSU.apvRomBank[GSU.vPrgBankReg];

    /* Set screen pointers */
    GSU.pvScreenBase = &GSU.pvRam[ USEX8(p[GSU_SCBR]) << 10 ];
    i = (int)(!!(p[GSU_SCMR] & 0x04));
    i |= ((int)(!!(p[GSU_SCMR] & 0x20))) << 1;
    GSU.vScreenHeight = GSU.vScreenRealHeight = avHeight[i];
    GSU.vMode = p[GSU_SCMR] & 0x03;
	
    if(i == 3)
		GSU.vScreenSize = 32768;
    else
		GSU.vScreenSize = GSU.vScreenHeight * 4 * avMult[GSU.vMode];
	
    if (GSU.vPlotOptionReg & 0x10)
		GSU.vScreenHeight = 256;// OBJ Mode (for drawing into sprites)
	
    if(GSU.pvScreenBase + GSU.vScreenSize > GSU.pvRam + (GSU.nRamBanks * 65536))
		GSU.pvScreenBase =  GSU.pvRam + (GSU.nRamBanks * 65536) - GSU.vScreenSize;

    GSU.pfPlot = fx_apfPlotTable[GSU.vMode];
    GSU.pfRpix = fx_apfPlotTable[GSU.vMode + 5];

    fx_ppfOpcodeTable[0x04c] = GSU.pfPlot;
    fx_ppfOpcodeTable[0x14c] = GSU.pfRpix;
    fx_ppfOpcodeTable[0x24c] = GSU.pfPlot;
    fx_ppfOpcodeTable[0x34c] = GSU.pfRpix;
	
	if(GSU.vMode != GSU.vPrevMode || GSU.vPrevScreenHeight != GSU.vScreenHeight || GSU.vSCBRDirty)
		fx_computeScreenPointers ();
	
	//fx_backupCache();
}

/*void fx_dirtySCBR()
{
	GSU.vSCBRDirty = TRUE;
}*/

void fx_computeScreenPointers ()
{
    int32_t i, j, condition, mask, result;
	uint32_t apvIncrement, vMode, xIncrement;
	GSU.vSCBRDirty = FALSE;

	/* Make a list of pointers to the start of each screen column*/
   vMode = GSU.vMode;
   condition = vMode - 2;
   mask = (condition | -condition) >> 31;
   result = (vMode & mask) | (3 & ~mask);
   vMode = result + 1;
   GSU.x[0] = 0;
   GSU.apvScreen[0] = GSU.pvScreenBase;
   apvIncrement = vMode << 4;

   if(GSU.vScreenHeight == 256)
   {
      GSU.x[16] = vMode << 12;
      GSU.apvScreen[16] = GSU.pvScreenBase + (vMode << 13);
      apvIncrement <<= 4;
      xIncrement = vMode << 4;

      for(i = 1, j = 17 ; i < 16 ; i++, j++)
      {
         GSU.x[i] = GSU.x[i - 1] + xIncrement;
         GSU.apvScreen[i] = GSU.apvScreen[i - 1] + apvIncrement;
         GSU.x[j] = GSU.x[j - 1] + xIncrement;
         GSU.apvScreen[j] = GSU.apvScreen[j - 1] + apvIncrement;
      }
   }
   else
   {
      xIncrement = (vMode * GSU.vScreenHeight) << 1;
      for(i = 1 ; i < 32 ; i++)
      {
         GSU.x[i] = GSU.x[i - 1] + xIncrement;
         GSU.apvScreen[i] = GSU.apvScreen[i - 1] + apvIncrement;
      }
   }
   GSU.vPrevMode = GSU.vMode;
   GSU.vPrevScreenHeight = GSU.vScreenHeight;
}

/*static void fx_writeRegisterSpaceAfterCheck(void)
{
   GSU.pvRegisters[30] = (uint8_t) R15;
   GSU.pvRegisters[31] = (uint8_t) (R15 >> 8);
}*/

static inline void fx_writeRegisterSpaceAfterUse()
{
    int i;
    uint8 *p = GSU.pvRegisters;
 
    for(i=0; i<15; i++)
    {
		*p++ = (uint8)GSU.avReg[i];
		*p++ = (uint8)(GSU.avReg[i] >> 8);
    }

    /* Update status register */
    if( USEX16(GSU.vZero) == 0 ) SF(Z);
    else CF(Z);
    if( GSU.vSign & 0x8000 ) SF(S);
    else CF(S);
    if(GSU.vOverflow >= 0x8000 || GSU.vOverflow < -0x8000) SF(OV);
    else CF(OV);
    if(GSU.vCarry) SF(CY);
    else CF(CY);
    
    p = GSU.pvRegisters;
    p[GSU_SFR] = (uint8)GSU.vStatusReg;
    p[GSU_SFR+1] = (uint8)(GSU.vStatusReg>>8);
    p[GSU_PBR] = (uint8)GSU.vPrgBankReg;
    p[GSU_ROMBR] = (uint8)GSU.vRomBankReg;
    p[GSU_RAMBR] = (uint8)GSU.vRamBankReg;
    p[GSU_CBR] = (uint8)GSU.vCacheBaseReg;
    p[GSU_CBR+1] = (uint8)(GSU.vCacheBaseReg>>8);
	
	//fx_restoreCache();
}

/* Reset the FxChip */
void FxReset(struct FxInit_s *psFxInfo)
{
    int i;
    static uint32 (**appfFunction[])(uint32) = {
	&fx_apfFunctionTable[0], };

    static void (**appfPlot[])() = {
	&fx_apfPlotTable[0], };

    static void (**appfOpcode[])() = {
	&fx_apfOpcodeTable[0], };

    /* Get function pointers for the current emulation mode */
    fx_ppfFunctionTable = appfFunction[psFxInfo->vFlags & 0x3];
    fx_ppfPlotTable = appfPlot[psFxInfo->vFlags & 0x3];
    fx_ppfOpcodeTable = appfOpcode[psFxInfo->vFlags & 0x3];
    
    /* Clear all internal variables */
    memset((uint8*)&GSU,0,sizeof(struct FxRegs_s));

    /* Set default registers */
    GSU.pvSreg = GSU.pvDreg = &R0;

    /* Set RAM and ROM pointers */
    GSU.pvRegisters = psFxInfo->pvRegisters;
    GSU.nRamBanks = psFxInfo->nRamBanks;
    GSU.pvRam = psFxInfo->pvRam;
    GSU.nRomBanks = psFxInfo->nRomBanks;
    GSU.pvRom = psFxInfo->pvRom;
    GSU.vPrevScreenHeight = ~0;
    GSU.vPrevMode = ~0;

    /* The GSU can't access more than 2mb (16mbits) */
    if(GSU.nRomBanks > 0x20)
	GSU.nRomBanks = 0x20;
    
    /* Clear FxChip register space */
    memset(GSU.pvRegisters,0,0x300);

    /* Set FxChip version Number */
    GSU.pvRegisters[0x3b] = 0;

    /* Make ROM bank table */
    for(i=0; i<256; i++)
    {
	uint32 b = i & 0x7f;
	if (b >= 0x40)
	{
	    if (GSU.nRomBanks > 1)
		b %= GSU.nRomBanks;
	    else
		b &= 1;

	    GSU.apvRomBank[i] = &GSU.pvRom[ b << 16 ];
	}
	else
	{
	    b %= GSU.nRomBanks * 2;
	    GSU.apvRomBank[i] = &GSU.pvRom[ (b << 16) + 0x200000];
	}
    }

    /* Make RAM bank table */
    for(i=0; i<4; i++)
    {
	GSU.apvRamBank[i] = &GSU.pvRam[(i % GSU.nRamBanks) << 16];
	GSU.apvRomBank[0x70 + i] = GSU.apvRamBank[i];
    }
    
    /* Start with a nop in the pipe */
    GSU.vPipe = 0x01;

    /* Set pointer to GSU cache */
    GSU.pvCache = &GSU.pvRegisters[0x100];

    //fx_readRegisterSpaceForCheck();
	R15 = GSU.pvRegisters[30];
	R15 |= ((uint32_t) GSU.pvRegisters[31]) << 8;
	//
	fx_readRegisterSpaceForUse();
}

static uint8 fx_checkStartAddress()
{
    /* Check if we start inside the cache 
    if(GSU.bCacheActive && R15 >= GSU.vCacheBaseReg && R15 < (GSU.vCacheBaseReg+512))
	return TRUE;*/
   
    /*  Check if we're in an unused area */
    if(GSU.vPrgBankReg >= 0x60 && GSU.vPrgBankReg <= 0x6f)
	return FALSE;

    if(GSU.vPrgBankReg >= 0x74)
	return FALSE;

    /* Check if we're in RAM and the RAN flag is not set */
    if (GSU.vPrgBankReg >= 0x70 && !(SCMR & (1 << 3)))
	return FALSE;

    /* If not, we're in ROM, so check if the RON flag is set */
    if(!(SCMR&(1<<4)))
	return FALSE;
    
    return TRUE;
}

/* Execute until the next stop instruction */
//int FxEmulate(uint32 nInstructions)
void S9xSuperFXExec ()
{
	bool8 address_valid;
	uint16 GSUStatus;
    
	uint32 nInstructions = (ROM_GLOBAL[0x3000 + GSU_CLSR] & 1) ? SuperFX.speedPerLinex2 : SuperFX.speedPerLine;
	
	/* Read registers and initialize GSU session */
    //fx_readRegisterSpaceForCheck();
	R15 = GSU.pvRegisters[30];
	R15 |= ((uint32_t) GSU.pvRegisters[31]) << 8;

   	/* Check if we start inside the cache*/
	if (GSU.bCacheActive && R15 >= GSU.vCacheBaseReg && R15 < (GSU.vCacheBaseReg + 512))
		address_valid = TRUE;
	else
		/* Check if the start address is valid*/
		address_valid = fx_checkStartAddress();
	
	if (!address_valid)
	{
		CF(G);
		//fx_writeRegisterSpaceAfterCheck();
		GSU.pvRegisters[30] = (uint8_t) R15;
		GSU.pvRegisters[31] = (uint8_t) (R15 >> 8);
		
		GSUStatus = ROM_GLOBAL[0x3000 + GSU_SFR] | (ROM_GLOBAL[0x3000 + GSU_SFR + 1] << 8);
		if ((GSUStatus & (FLG_G | FLG_IRQ)) == FLG_IRQ)
			// Trigger a GSU IRQ.
			S9xSetIRQ (GSU_IRQ_SOURCE);
		
		return;	
	}

	fx_readRegisterSpaceForUse();
	
	/* Execute GSU session */
	CF(IRQ);

	/*if(GSU.bBreakPoint)
	vCount = fx_ppfFunctionTable[FX_FUNCTION_RUN_TO_BREAKPOINT](nInstructions);
	else
	vCount = fx_ppfFunctionTable[FX_FUNCTION_RUN](nInstructions);*/
			
	GSU.vCounter = nInstructions;
	READR14;
    while (TF(G) && GSU.vCounter-- > 0)
	{
		/* Execute instruction from the pipe, and fetch next byte to the pipe*/
		uint32	vOpcode = (uint32) PIPE;
		FETCHPIPE;
		(*fx_ppfOpcodeTable[(GSU.vStatusReg & 0x300) | vOpcode])();
	}
	
    /* Store GSU registers */
    //fx_writeRegisterSpaceAfterCheck();
	GSU.pvRegisters[30] = (uint8_t) R15;
	GSU.pvRegisters[31] = (uint8_t) (R15 >> 8);
	//
	fx_writeRegisterSpaceAfterUse();
	
	GSUStatus = ROM_GLOBAL[0x3000 + GSU_SFR] | (ROM_GLOBAL[0x3000 + GSU_SFR + 1] << 8);
	if ((GSUStatus & (FLG_G | FLG_IRQ)) == FLG_IRQ)
		S9xSetIRQ (GSU_IRQ_SOURCE);
		
    /* Check for error code */
    /*if(GSU.vErrorCode)
	return GSU.vErrorCode;
    else*/
	//return vCount;
}

/* Breakpoints */
/*void FxBreakPointSet(uint32 vAddress)
{
    GSU.bBreakPoint = TRUE;
    GSU.vBreakPoint = USEX16(vAddress);
}*/

/*void FxBreakPointClear()
{
    GSU.bBreakPoint = FALSE;
}*/

/* Step by step execution */
/*int FxStepOver(uint32 nInstructions)
{
    uint32 vCount;
    fx_readRegisterSpace();

    // Check if the start address is valid 
    if(!fx_checkStartAddress())
    {
	CF(G);
#if 0
	GSU.vIllegalAddress = (GSU.vPrgBankReg << 24) | R15;
	return FX_ERROR_ILLEGAL_ADDRESS;
#else
	return 0;
#endif
    }
    
    if( PIPE >= 0xf0 )
	GSU.vStepPoint = USEX16(R15+3);
    else if( (PIPE >= 0x05 && PIPE <= 0x0f) || (PIPE >= 0xa0 && PIPE <= 0xaf) )
	GSU.vStepPoint = USEX16(R15+2);
    else
	GSU.vStepPoint = USEX16(R15+1);
    vCount = fx_ppfFunctionTable[FX_FUNCTION_STEP_OVER](nInstructions);
    fx_writeRegisterSpace();
    if(GSU.vErrorCode)
	return GSU.vErrorCode;
    else
	return vCount;
}*/

/* Errors */
/*int FxGetErrorCode()
{
    return GSU.vErrorCode;
}*/

/*int FxGetIllegalAddress()
{
    return GSU.vIllegalAddress;
}*/

/* Access to internal registers */
/*uint32 FxGetColorRegister()
{
    return GSU.vColorReg & 0xff;
}

uint32 FxGetPlotOptionRegister()
{
    return GSU.vPlotOptionReg & 0x1f;
}

uint32 FxGetSourceRegisterIndex()
{
    return GSU.pvSreg - GSU.avReg;
}

uint32 FxGetDestinationRegisterIndex()
{
    return GSU.pvDreg - GSU.avReg;
}

uint8 FxPipe()
{
    return GSU.vPipe;
}*/

