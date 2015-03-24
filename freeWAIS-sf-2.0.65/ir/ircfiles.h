/* Wide AREA INFORMATION SERVER SOFTWARE:
   No guarantees or restrictions.  See the readme file for the full standard
   disclaimer.

   Brewster@think.com
 *
 * $Log: ircfiles.h,v $
 * Revision 2.0  1995/09/08 07:59:24  pfeifer
 * Metaconfig baseline
 *
 * Revision 1.5  1994/03/28  09:29:21  pfeifer
 * Integrated Essence
 *
 * Revision 1.4  1994/03/08  21:06:16  pfeifer
 * Patchlevel 04
 *
 * Revision 1.3  93/07/21  18:43:42  warnock
 * added functions for indexing listserv archives and STELAR-specific stuff
 * 
 * Revision 1.2  93/07/19  16:31:58  warnock
 * Added document type URL from Nathan.Torkington@vuw.ac.nz
 * 
 * Revision 1.1  1993/02/16  15:05:35  freewais
 * Initial revision
 *
 * Revision 1.13  92/05/06  17:30:09  jonathan
 * Added prototype for filename_finish_header_function.
 * 
 * Revision 1.12  92/04/25  21:13:47  brewster
 * added ziff
 * 
 * Revision 1.11  92/03/22  18:38:42  brewster
 * added objective C filter
 * 
 * Revision 1.10  92/02/20  09:50:01  jonathan
 * Added bibtex and nhyp filters from S.P.vandeBurgt@research.ptt.nl.
 * 
 */

/* customizations for indexing specialize files */

#ifndef IRCFILES_H
#define IRCFILES_H

#include "cdialect.h"

#ifdef __cplusplus
/* declare these as C style functions */
extern "C"
	{
#endif /* def __cplusplus */
#ifdef FIELDS
boolean fields_separator_function _AP((char *line));
void fields_header_function _AP((char *line));
void fields_finish_header_function _AP((char *header));
long fields_date_function _AP((char *line));
#endif
#ifdef BIBDB
/* formfeed seperated files */
boolean formfeed_separator_function _AP((char *line));
void formfeed_header_function _AP((char *line));
void formfeed_finish_header_function _AP((char *header));

/* formfeed seperated files e.g. steve files */
boolean bibdb_separator_function _AP((char *line));
void bibdb_header_function _AP((char *line));
void bibdb_finish_header_function _AP((char *header));
long bibdb_date_function _AP((char *line));

/* irlist digest */
boolean irlist_separator_function _AP((char *line));
void irlist_header_function _AP((char *line));
long irlist_date_function _AP((char *line));

/* formfeed seperated files e.g. bibinf */
boolean bibinf_separator_function _AP((char *line));
void bibinf_header_function _AP((char *line));
void bibinf_finish_header_function _AP((char *header));
long bibinf_date_function _AP((char *line));
#endif

#ifdef STELAR
/* stelar  -- aw3 */
boolean stelar_separator_function _AP((char *line));
void stelar_header_function _AP((char *line));
void stelar_finish_header_function _AP((char *header));

/* AAS Meeting (LaTeX) abstracts files */
boolean aasab_separator_function _AP((char *line));
void aasab_header_function _AP((char *line));
void aasab_finish_header_function _AP((char *header));

#endif /* STELAR */

#ifdef BIO
/* genbank flat files -- dgg*/
boolean genbank_separator_function _AP((char *line));
void genbank_header_function _AP((char *line));
void genbank_finish_header_function _AP((char *header));
long genbank_date_function _AP((char *line));

/* embl flat files -- dgg*/
boolean embl_separator_function _AP((char *line));
void embl_header_function _AP((char *line));
void embl_finish_header_function _AP((char *header));
long embl_date_function _AP((char *line));

/* pir flat files -- dgg*/
boolean pir_separator_function _AP((char *line));
void pir_header_function _AP((char *line));
void pir_finish_header_function _AP((char *header));
long pir_date_function _AP((char *line));

/* prosite protein doc & dat files - dgg */
boolean prositedoc_separator_function _AP((char *line));
void prositedoc_header_function _AP((char *line));
void prositedoc_finish_header_function _AP((char *header));

boolean prositedat_separator_function _AP((char *line));
void prositedat_header_function _AP((char *line));
void prositedat_finish_header_function _AP((char *header));

/* Bionet.journals table of contents format */
boolean biojournal_separator_function _AP((char *line));
void biojournal_header_function _AP((char *line));
void biojournal_finish_header_function _AP((char *header));

/* drosophila redbook -dgg */
boolean redbook_separator_function _AP((char *line));
void redbook_header_function _AP((char *line));
void redbook_finish_header_function _AP((char *header));

/* drosophila flybase -dgg */
boolean flybase_separator_function _AP((char *line));
void flybase_header_function _AP((char *line));
void flybase_finish_header_function _AP((char *header));

boolean din_separator_function _AP((char *line));
void din_finish_header_function _AP((char *header));
void din_header_function _AP((char *line));

#endif /* BIO */

/* groliers encyclopedia */
boolean groliers_separator_function _AP((char *line));
void groliers_header_function _AP((char *line));
void groliers_finish_header_function _AP((char *header));

/* unix mail and rmail files */
boolean mail_separator_function _AP((char *line));
boolean rmail_separator_function _AP((char *line));
void mail_header_function _AP((char *line));
void mail_finish_header_function _AP((char *header));
boolean mail_or_rmail_separator _AP((char *line));
long mail_date_function _AP((char *line));

/* mail digest files  */
boolean mail_digest_separator_function _AP((char *line));
boolean mh_bboard_separator_function _AP((char *line));

/* listserv digest files */
boolean listserv_digest_separator_function _AP((char *line));
void listserv_header_function _AP((char *line));
long listserv_date_function _AP((char *line));
void listserv_finish_header_function _AP((char *header));

/* library catalog files */
boolean catalog_separator_function _AP((char *line));
void catalog_header_function _AP((char *line));
void catalog_finish_header_function _AP((char *header));

/* biology abstracts files */
boolean bio_separator_function _AP((char *line));
void bio_header_function _AP((char *line));
void bio_finish_header_function _AP((char *header));

/* cm application files */
boolean cmapp_separator_function _AP((char *line));
void cmapp_header_function _AP((char *line));
void cmapp_finish_header_function _AP((char *header));

/* jargon files */
boolean jargon_separator_function _AP((char *line));
void jargon_header_function _AP((char *line));
void jargon_finish_header_function _AP((char *header));

/* internet resource guide */
boolean irg_separator_function _AP((char *line));
void irg_header_function _AP((char *line));
void irg_finish_header_function _AP((char *header));

/* dash seperated files */
boolean dash_separator_function _AP((char *line));
void dash_header_function _AP((char *line));
void dash_finish_header_function _AP((char *header));

/* one_line seperated files */
boolean one_line_separator_function _AP((char *line));
void one_line_header_function _AP((char *line));
void one_line_finish_header_function _AP((char *header));

/* blank line seperated files (para) */
boolean para_separator_function _AP((char *line));
void para_header_function _AP((char *line));
void para_finish_header_function _AP((char *header));

/* files from the Seeker system (dash separated, headline: marker) */
boolean seeker_separator_function _AP((char *line));
void seeker_header_function _AP((char *line));
void seeker_finish_header_function _AP((char *header));

/* files from the Rlin system (dash separated, headline: marker) */
boolean rlin_separator_function _AP((char *line));
void rlin_header_function _AP((char *line));
void rlin_finish_header_function _AP((char *header));

/* files saved by the RN newsreader */
boolean rn_separator_function _AP((char *line));

/* The GNU Emacs Texinfo documentation system */
boolean emacs_info_separator_function _AP((char *line));
void emacs_info_header_function _AP((char *line));
void emacs_info_finish_header_function _AP((char *headline));

/* files in medline format */
boolean medline_separator_function _AP((char *line));
void medline_header_function _AP((char *line));
void medline_finish_header_function _AP((char *header));

/* files in refer format */
boolean refer_separator_function _AP((char *line));
void refer_header_function _AP((char *line));
void refer_finish_header_function _AP((char *header));

/* first line of the file is the headline */
boolean first_line_separator_function _AP((char *line));
void first_line_header_function _AP((char *line));
void first_line_finish_header_function _AP((char *header));

/* BibTeX files */
boolean bibtex_separator_function _AP((char *line));
void bibtex_header_function _AP((char *line));
void bibtex_finish_header_function _AP((char *header));

/* hypertext files */
boolean nhyp_separator_function _AP((char *line));
void nhyp_header_function _AP((char *line));
void nhyp_finish_header_function _AP((char *header));

#ifdef NeXT
/* objective-C code */
boolean wobjc_separator_function _AP((char *line));
void wobjc_header_function _AP((char *line));
void wobjc_finish_header_function _AP((char *header));
#endif /* def NeXT */

/* Ziff files */
boolean ziff_separator_function _AP((char *line));
void ziff_header_function _AP((char *line));
void ziff_finish_header_function _AP((char *header));

#ifdef ESSENCE
/* CU-Boulder, Essence summaries, 10/8/92, hardy@cs.colorado.edu */
boolean essence_separator_function _AP((char *line));
void essence_header_function _AP((char *line));
void essence_finish_header_function _AP((char *header));

#endif /* ESSENCE */
void filename_finish_header_function _AP((char *header));


char *cleanHeadline _AP((char *header));
extern char *URL_prefix;
extern char *URL_trim;



#ifdef __cplusplus
	}
#endif /* def __cplusplus */

#endif /* ndef IRCFILES_H */
