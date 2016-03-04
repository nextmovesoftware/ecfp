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

#include "openbabel/mol.h"
#include "openbabel/obiter.h"

#include "ecfp.h"

#define mix32(a,b,c) \
{ \
  a -= b; a -= c; a ^= (c>>13); \
  b -= c; b -= a; b ^= (a<<8); \
  c -= a; c -= b; c ^= (b>>13); \
  a -= b; a -= c; a ^= (c>>12);  \
  b -= c; b -= a; b ^= (a<<16); \
  c -= a; c -= b; c ^= (b>>5); \
  a -= b; a -= c; a ^= (c>>3);  \
  b -= c; b -= a; b ^= (a<<10); \
  c -= a; c -= b; c ^= (b>>15); \
}

static unsigned int ECFPHash(unsigned char *ptr, unsigned int length)
{
  unsigned int a = 0;
  unsigned int b = 0;
  unsigned int c = 0;

  unsigned int len = length;
  while (len >= 12) {
    a += ptr[0] + ((unsigned int)ptr[1]<<8)
                + ((unsigned int)ptr[2]<<16)
                + ((unsigned int)ptr[3]<<24);
    b += ptr[4] + ((unsigned int)ptr[5]<<8)
                + ((unsigned int)ptr[6]<<16)
                + ((unsigned int)ptr[7]<<24);
    c += ptr[8] + ((unsigned int)ptr[9]<<8)
                + ((unsigned int)ptr[10]<<16)
                + ((unsigned int)ptr[11]<<24);
    mix32(a,b,c);
    ptr += 12;
    len -= 12;
  }

  c += length;
  switch (len) {
  case 11: c += ((unsigned int)ptr[10]<<24);
  case 10: c += ((unsigned int)ptr[9]<<16);
  case  9: c += ((unsigned int)ptr[8]<<8);
  case  8: b += ((unsigned int)ptr[7]<<24);
  case  7: b += ((unsigned int)ptr[6]<<16);
  case  6: b += ((unsigned int)ptr[5]<<8);
  case  5: b += ptr[4];
  case  4: a += ((unsigned int)ptr[3]<<24);
  case  3: a += ((unsigned int)ptr[2]<<16);
  case  2: a += ((unsigned int)ptr[1]<<8);
  case  1: a += ptr[0];
  }
  mix32(a,b,c);
  return c;
}


static unsigned int ECFPHash(std::vector<unsigned int> &v)
{
  unsigned int len = (unsigned int)v.size();
  unsigned int a = 0;
  unsigned int b = 0;
  unsigned int c = 0;
  unsigned int i = 0;

  while (i+2 < len) {
    a += v[i];
    b += v[i+1];
    c += v[i+2];
    mix32(a,b,c);
    i += 3;
  }
  c += len;
  switch (len - i) {
  case 2:  b += v[i+1];
  case 1:  a += v[i];
  }
  mix32(a,b,c);
  return c;
}


struct AtomInfo {
  unsigned int e[5]; // hashcodes for ecfp0, 2, 4... This will segfault at ECFP12
  
  // std::vector<unsigned int> b[4]; // for duplicate removal as described in paper?
};

struct NborInfo {
  unsigned int order;
  unsigned int idx;

  NborInfo(unsigned int o, unsigned int i) : order(o), idx(i) {}
  bool operator < (const NborInfo &x) const
  {
    if (order != x.order)
      return order < x.order;
    return idx < x.idx;
  }
};

static void ECFPPass(OpenBabel::OBMol &mol,
                     AtomInfo *ainfo, unsigned int pass)
{
  FOR_ATOMS_OF_MOL(atom, mol) {
    if (atom->IsHydrogen())
      continue;
    OpenBabel::OBAtom* aptr = &(*atom);
    unsigned int idx = aptr->GetIdx()-1;
    AtomInfo *ptr = &ainfo[idx];

    std::vector<NborInfo> nbrs;
    FOR_BONDS_OF_ATOM(bptr, aptr) {
      OpenBabel::OBAtom* nptr = bptr->GetNbrAtom(aptr);
      if (nptr->IsHydrogen())
        continue;
      unsigned int order;
      if (!bptr->IsAromatic()) {
        switch (bptr->GetBondOrder()) {
        case 2:  order = 2;  break;
        case 3:  order = 3;  break;
        default: order = 1;
        }
      } else order = 4;

      unsigned int nidx = nptr->GetIdx();
      nbrs.push_back(NborInfo(order,ainfo[nidx].e[pass-1]));
      // for duplicate removal as described in paper (?)
      //if (pass == 1)
      //  ptr->b[pass-1].push_back(bptr->GetIdx());
    }
    std::sort(nbrs.begin(),nbrs.end());

    std::vector<unsigned int> vint;
    vint.push_back(pass);
    vint.push_back(ptr->e[pass-1]);
    std::vector<NborInfo>::const_iterator ni;
    for (ni=nbrs.begin(); ni!=nbrs.end(); ++ni) {
      vint.push_back(ni->order);
      vint.push_back(ni->idx);
    }
    ptr->e[pass] = ECFPHash(vint);
  }
}


static void ECFPInsert(ECFP_Fingerprint &fp, unsigned int val)
{
  std::vector<unsigned int>::const_iterator i;
  for (i=fp.v.begin(); i!=fp.v.end(); ++i)
    if (*i == val)
      return;
  fp.v.push_back(val);
}


static void ECFPFirstPass(OpenBabel::OBMol &mol,
                          AtomInfo *ainfo)
{
  unsigned char buffer[8];

  /* First Pass: ECFP_0 */
  FOR_ATOMS_OF_MOL(atom, mol) {
    if (atom->IsHydrogen())
      continue;
    OpenBabel::OBAtom* aptr = &(*atom);
    unsigned int idx = aptr->GetIdx()-1;
    buffer[0] = aptr->GetHvyValence(); // degree of heavy atom connections
    buffer[1] = aptr->BOSum() - aptr->ExplicitHydrogenCount(); // valence of heavy atom connections
    buffer[2] = aptr->GetAtomicNum();
    buffer[3] = (unsigned char)aptr->GetIsotope();
    buffer[4] = (unsigned char)aptr->GetFormalCharge();
    buffer[5] = (unsigned char)(aptr->ExplicitHydrogenCount() + aptr->ImplicitHydrogenCount());
    buffer[6] = aptr->IsInRing() ? 1 : 0;
    buffer[7] = 0;  // aptr->IsAromatic() ? 1 : 0;
    ainfo[idx].e[0] = ECFPHash(buffer,8);
  }
}


void GenerateECFP(ECFP_Fingerprint &fp,
                  OpenBabel::OBMol &mol,
                  unsigned int radius, bool keepdups)
{
  unsigned int pass;

  fp.v.clear();

  unsigned int count = mol.NumAtoms();
  if (count == 0) return;

  AtomInfo *ainfo = new AtomInfo[count];

  ECFPFirstPass(mol,ainfo);
  for (pass=1; pass<=radius; pass++)
    ECFPPass(mol,ainfo,pass);

  // Duplicate removal - this is a simplified version of what's in the paper
  FOR_ATOMS_OF_MOL(atom, mol) {
    if (atom->IsHydrogen())
      continue;
    unsigned int idx = atom->GetIdx()-1;
    if (keepdups) {
      for (pass=0; pass<=radius; pass++)
        fp.v.push_back(ainfo[idx].e[pass]);
    } else
      for (pass=0; pass<=radius; pass++)
        ECFPInsert(fp,ainfo[idx].e[pass]);
  }

  delete[] ainfo;

  std::sort(fp.v.begin(),fp.v.end());
}

void Tanimoto(const std::vector<unsigned int> &a, const std::vector<unsigned int> &b, unsigned int &num, unsigned int &denom)
{
  unsigned int as = (unsigned int)a.size();
  unsigned int bs = (unsigned int)b.size();

  denom = 0;
  num = 0;

  while (as > 0 && bs > 0) {
      unsigned int ai = a[as-1];
      unsigned int bi = b[bs-1];
      if (ai > bi)
      {
        denom++;
        as--;
      }
      else if (ai < bi)
      {
        denom++;
        bs--;
      }
      else
      {
        denom++;
        num++;
        as--;
        bs--;
      }
    }
  denom += as + bs;
}


double Tanimoto(const ECFP_Fingerprint &a, const ECFP_Fingerprint &b)
{
  unsigned int num, denom;
  Tanimoto(a.v,b.v,num,denom);
  return denom != 0 ? (double)num/denom : 0.0;
}
