/*
 * $Id$
 * 
 *                This source code is part of
 * 
 *                 G   R   O   M   A   C   S
 * 
 *          GROningen MAchine for Chemical Simulations
 * 
 *                        VERSION 3.3.3
 * Written by David van der Spoel, Erik Lindahl, Berk Hess, and others.
 * Copyright (c) 1991-2000, University of Groningen, The Netherlands.
 * Copyright (c) 2001-2008, The GROMACS development team,
 * check out http://www.gromacs.org for more information.

 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * If you want to redistribute modifications, please consider that
 * scientific software is very special. Version control is crucial -
 * bugs must be traceable. We will be happy to consider code for
 * inclusion in the official distribution, but derived work must not
 * be called official GROMACS. Details are found in the README & COPYING
 * files - if they are missing, get the official version at www.gromacs.org.
 * 
 * To help us fund GROMACS development, we humbly ask that you cite
 * the papers on the package - you can find them in the top README file.
 * 
 * For more info, check our website at http://www.gromacs.org
 * 
 * And Hey:
 * Groningen Machine for Chemical Simulation
 */
/* This file is completely threadsafe - keep it that way! */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "typedefs.h"
#include "bondf.h"
#include "disre.h"
#include "dihre.h"
#include "orires.h"

#define  def_bonded(str,lstr,nra,nrpa,nrpb,ind,func)\
   {str,lstr,(nra),(nrpa),(nrpb),IF_BOND,                        (ind),(func)}

#define   def_angle(str,lstr,nra,nrpa,nrpb,ind,func)\
   {str,lstr,(nra),(nrpa),(nrpb),IF_BOND | IF_ATYPE,(ind),(func)}
   
#define    def_bond(str,lstr,nra,nrpa,nrpb,ind,func)\
   {str,lstr,(nra),(nrpa),(nrpb),IF_BOND | IF_CHEMBOND | IF_BTYPE,(ind),(func)}

#define  def_bondnb(str,lstr,nra,nrpa,nrpb,ind,func)\
   {str,lstr,(nra),(nrpa),(nrpb),IF_BOND | IF_CHEMBOND,(ind),(func)}

#define   def_vsite(str,lstr,nra,nrpa)\
   {str,lstr,(nra),(nrpa),     0,IF_VSITE,                  -1, unimplemented}
   
#define     def_shk(str,lstr,nra,nrpa,nrpb)\
   {str,lstr,(nra),(nrpa),(nrpb),IF_CONSTRAINT,             -1, unimplemented}

#define   def_shkcb(str,lstr,nra,nrpa,nrpb)\
   {str,lstr,(nra),(nrpa),(nrpb),IF_CONSTRAINT | IF_CHEMBOND,-1, unimplemented}
   
#define      def_nb(str,lstr,nra, nrp)\
   {str,lstr,(nra), (nrp),     0,IF_NULL,                    -1,unimplemented}
   
#define    def_nofc(str,lstr)\
   {str,lstr,    0,     0,     0,IF_NULL,                    -1,unimplemented}

/* this MUST correspond to the enum in include/types/idef.h */
const t_interaction_function interaction_function[F_NRE]=
{
  def_bond   ("BONDS",    "Bond",            2, 2, 2,  eNR_BONDS,  bonds         ),
  def_bond   ("G96BONDS", "G96Bond",         2, 2, 2,  eNR_BONDS,  g96bonds      ),
  def_bond   ("MORSE",    "Morse",           2, 3, 0,  eNR_MORSE,  morse_bonds   ),
  def_bond   ("CUBICBONDS","Cubic Bonds",    2, 3, 0,  eNR_CUBICBONDS, cubic_bonds),
  def_bondnb ("CONNBONDS","Connect Bonds",   2, 0, 0,  0,      unimplemented     ),
  def_bonded ("HARMONIC", "Harmonic Pot.",   2, 2, 2,  eNR_BONDS,  bonds         ),
  def_bondnb ("FENEBONDS", "FENE Bonds",     2, 2, 0,  eNR_FENEBONDS, FENE_bonds  ),
  def_angle  ("ANGLES",   "Angle",           3, 2, 2,  eNR_ANGLES, angles        ),
  def_angle  ("G96ANGLES","G96Angle",        3, 2, 2,  eNR_ANGLES, g96angles     ),
  def_angle  ("CROSS_BOND_BOND", "Bond-Cross", 3, 3, 0,0,          cross_bond_bond ),
  def_angle  ("CROSS_BOND_ANGLE","BA-Cross",   3, 4, 0,0,          cross_bond_angle ),
  def_angle  ("UREY_BRADLEY","U-B",          3, 4, 4,  0,          urey_bradley ),
  def_angle  ("QANGLES","Quartic Angles",    3, 6, 0,  eNR_QANGLES, quartic_angles ),
  def_bonded ("PDIHS",    "Proper Dih.",     4, 3, 3,  eNR_PROPER, pdihs         ),
  def_bonded ("RBDIHS",   "Ryckaert-Bell.",  4, 6, 6,  eNR_RB, rbdihs            ),
  def_bonded ("FOURDIHS", "Fourier Dih.",    4, 4, 4,  eNR_FOURDIH, rbdihs       ),
  def_bonded ("IDIHS",    "Improper Dih.",   4, 2, 2,  eNR_IMPROPER,idihs        ),
  def_bonded ("PIDIHS",   "Improper Dih.",   4, 3, 3,  eNR_PROPER, pdihs         ),
  def_bonded ("LJ14",     "LJ-14",           2, 2, 2,  eNR_NB14,   unimplemented ),
  def_nofc   ("COUL14",   "Coulomb-14"                                           ),
  def_nb     ("LJ_SR",    "LJ (SR)",         2, 2                                ),
  def_nb     ("BHAM",     "Buck.ham (SR)",   2, 3                                ),
  def_nofc   ("LJ_LR",    "LJ (LR)"                                              ),
  def_nofc   ("BHAM_LR",  "Buck.ham (LR)"                                        ),
  def_nofc   ("DISPCORR", "Disper. corr."                                        ),
  def_nofc   ("COUL_SR",  "Coulomb (SR)"                                         ),
  def_nofc   ("COUL_LR",  "Coulomb (LR)"                                         ),
  def_nofc   ("RF_EXCL",  "RF excl."                                             ),
  def_nofc   ("COUL_RECIP", "Coul. recip."                                       ),
  def_bondnb ("POLARIZATION", "Polarization",2, 1, 0,  0,          polarize      ),
  def_bonded ("WATERPOL", "Water Pol.",      5, 6, 0,  eNR_WPOL,   water_pol     ),
  def_bonded ("THOLE",    "Thole Pol.",      4, 3, 0,  eNR_THOLE,  thole_pol     ),
  def_bonded ("POSRES",   "Position Rest.",  1, 3, 3,  eNR_POSRES, posres        ),
  def_bonded ("DISRES",   "Dis. Rest.",      2, 6, 0,  eNR_DISRES, ta_disres     ),
  def_nofc   ("DRVIOL",   "D. R. Viol. (nm)"                                     ),    
  def_bonded ("ORIRES",   "Orient. Rest.",   2, 6, 0,  eNR_ORIRES, orires        ),
  def_nofc   ("ORDEV",    "Ori. R. RMSD"                                         ),  
  def_bonded ("ANGRES",   "Angle Rest.",     4, 3, 3,  eNR_ANGRES, angres        ),
  def_bonded ("ANGRESZ",  "Angle Rest. Z",   2, 3, 3,  eNR_ANGRESZ,angresz       ),
  def_bonded ("DIHRES",   "Dih. Rest.",      4, 5, 0,  eNR_DIHRES, ta_dihres     ),
  def_nofc   ("DIHVIOL",  "Dih. Rest. viol."                                     ),    
  def_shkcb  ("CONSTR",   "Constraint",      2, 1, 1                             ),
  def_shk    ("CONSTRNC", "Constr. No Conn.",2, 1, 1                             ),
  def_shkcb  ("SETTLE",   "Settle",          1, 2, 0                             ),
  def_vsite  ("VSITE2",   "Virtual site 2",  3, 1                                ),
  def_vsite  ("VSITE3",   "Virtual site 3",  4, 2                                ),
  def_vsite  ("VSITE3FD", "Virtual site 3fd",4, 2                                ),
  def_vsite  ("VSITE3FAD","Virtual site 3fad",4, 2                               ),
  def_vsite  ("VSITE3OUT","Virtual site 3out",4, 3                               ),
  def_vsite  ("VSITE4FD", "Virtual site 4fd",5, 3                                ),
  def_nofc   ("EQM",      "Quantum En."      ),
  def_nofc   ("EPOT",     "Potential"        ),
  def_nofc   ("EKIN",     "Kinetic En."      ),
  def_nofc   ("ETOT",     "Total Energy"     ),
  def_nofc   ("TEMP",     "Temperature"      ),
  def_nofc   ("PRES",     "Pressure (bar)"   ),
  def_nofc   ("DV/DL",    "dVpot/dlambda"    ),
  def_nofc   ("DK/DL",    "dEkin/dlambda"    )
};

bool have_interaction(t_idef *idef,int ftype)
{
  int i;
  
  for(i=0; (i<idef->ntypes); i++)
    if (idef->functype[i] == ftype)
      return TRUE;
  return FALSE;
}
