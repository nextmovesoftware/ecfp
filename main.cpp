/**********************************************************************
Copyright (C) 2016 by NextMove Software

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 2 of the License.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
***********************************************************************/

#include <openbabel/mol.h>
#include <openbabel/obconversion.h>

#include "ecfp.h"

using namespace OpenBabel;

#define ECFP4 0

void PrintFP(const ECFP_Fingerprint &fp)
{
  printf("hashcodes: ");
  for(int i=0; i<fp.v.size(); ++i) 
    printf("%d ", fp.v[i]);
  printf("\n");
}

int main()
{
  OBConversion conv;
  bool ok = conv.SetInFormat("smi");
  if (!ok) {
    printf("Was not able to find smi format\n");
    exit(1);
  }


  OBMol mol;
  conv.ReadString(&mol, "c1c(I)cccc1(=O)CCCNC(=O)Cl");
  OBMol molb;
  conv.ReadString(&molb, "c1ccccc1(=O)CCCNC(=O)Cl");

  ECFP_Fingerprint fp, fpb, fpc;
  GenerateECFP(fp, mol, ECFP4);
  PrintFP(fp);
  GenerateECFP(fpb, molb, ECFP4);
  printf("Tanimoto is %.2f\n", Tanimoto(fp, fpb));

  // NB: fp should be invariant to adding hydrogens!
  mol.AddHydrogens();
  GenerateECFP(fpc, mol, ECFP4);
  PrintFP(fpc);

 return 0;
}