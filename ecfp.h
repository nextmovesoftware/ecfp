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

#include <vector>

struct ECFP_Fingerprint
{
  std::vector<unsigned int> v;
};

void GenerateECFP(ECFP_Fingerprint &fp, OpenBabel::OBMol &mol, unsigned int radius, bool keepdups=false);

double Tanimoto(const ECFP_Fingerprint &a, const ECFP_Fingerprint &b);
