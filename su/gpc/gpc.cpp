// gpc.cpp: implementation of the gpc class.
//
//////////////////////////////////////////////////////////////////////

#include "gpc.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

gpc::gpc()
{
}

gpc::~gpc()
{
}

void gpc::gpc_free_polygon(gpc_polygon *p)
{
	int c;

	for (c = 0; c < p->num_contours; c++)
		FREE(p->contour[c].vertex);
	FREE(p->hole);
	FREE(p->contour);
	// p->hole = 0;
	// p->contour = 0;
	p->num_contours = 0;
}

void gpc::gpc_read_polygon(FILE *fp, int read_hole_flags, gpc_polygon *p)
{
	int c, v;

	fscanf(fp, "%d", &(p->num_contours));
	MALLOCINT(p->hole, p->num_contours * sizeof(int),
			  "hole flag array creation");
	MALLOCGPC(p->contour, p->num_contours * sizeof(gpc_vertex_list), "contour creation");
	for (c = 0; c < p->num_contours; c++)
	{
		fscanf(fp, "%d", &(p->contour[c].num_vertices));

		if (read_hole_flags)
			fscanf(fp, "%d", &(p->hole[c]));
		else
			p->hole[c] = FALSE; /* Assume all contours to be external */

		MALLOCGPCV(p->contour[c].vertex, p->contour[c].num_vertices * sizeof(gpc_vertex), "vertex creation");
		for (v = 0; v < p->contour[c].num_vertices; v++)
			fscanf(fp, "%lf %lf", &(p->contour[c].vertex[v].x),
				   &(p->contour[c].vertex[v].y));
	}
}

void gpc::gpc_input_polygon(int read_hole_flags, gpc_polygon *p, double *input)
{
	int c, v;
	double *in;

	in = input;

	p->num_contours = (int)*(in++);
	MALLOCINT(p->hole, p->num_contours * sizeof(int),
			  "hole flag array creation");
	MALLOCGPC(p->contour, p->num_contours * sizeof(gpc_vertex_list), "contour creation");
	for (c = 0; c < p->num_contours; c++)
	{
		p->contour[c].num_vertices = (int)*(in++);

		if (read_hole_flags)
			p->hole[c] = (int)*(in++);
		else
			p->hole[c] = FALSE; /* Assume all contours to be external */

		MALLOCGPCV(p->contour[c].vertex, p->contour[c].num_vertices * sizeof(gpc_vertex), "vertex creation");
		for (v = 0; v < p->contour[c].num_vertices; v++)
		{
			p->contour[c].vertex[v].x = *(in++);
			p->contour[c].vertex[v].y = *(in++);
		}
	}
}

void gpc::gpc_write_polygon(FILE *fp, int write_hole_flags, gpc_polygon *p)
{
	int c, v;

	fprintf(fp, "%d\n", p->num_contours);
	for (c = 0; c < p->num_contours; c++)
	{
		fprintf(fp, "%d\n", p->contour[c].num_vertices);

		if (write_hole_flags)
			fprintf(fp, "%d\n", p->hole[c]);

		for (v = 0; v < p->contour[c].num_vertices; v++)
			fprintf(fp, "% .*lf % .*lf\n",
					DBL_DIG, p->contour[c].vertex[v].x,
					DBL_DIG, p->contour[c].vertex[v].y);
	}
}

void gpc::gpc_add_contour(gpc_polygon *p, gpc_vertex_list *new_contour, int hole)
{
	int *extended_hole, c, v;
	gpc_vertex_list *extended_contour;

	/* Create an extended hole array */
	MALLOCINT(extended_hole, (p->num_contours + 1) * sizeof(int), "contour hole addition");

	/* Create an extended contour array */
	MALLOCGPC(extended_contour, (p->num_contours + 1) * sizeof(gpc_vertex_list), "contour addition");

	/* Copy the old contour and hole data into the extended arrays */
	for (c = 0; c < p->num_contours; c++)
	{
		extended_hole[c] = p->hole[c];
		extended_contour[c] = p->contour[c];
	}

	/* Copy the new contour and hole onto the end of the extended arrays */
	c = p->num_contours;
	extended_hole[c] = hole;
	extended_contour[c].num_vertices = new_contour->num_vertices;
	MALLOCGPCV(extended_contour[c].vertex, new_contour->num_vertices * sizeof(gpc_vertex), "contour addition");
	for (v = 0; v < new_contour->num_vertices; v++)
		extended_contour[c].vertex[v] = new_contour->vertex[v];

	/* Dispose of the old contour */
	FREE(p->contour);
	FREE(p->hole);

	/* Update the polygon information */
	p->num_contours++;
	p->hole = extended_hole;
	p->contour = extended_contour;
}

void gpc::gpc_polygon_clip(gpc_op op, gpc_polygon *subj, gpc_polygon *clip,
						   gpc_polygon *result)
{
	sb_tree *sbtree = nullptr;
	it_node *it = nullptr, *intersect;
	edge_node *edge, *prev_edge, *next_edge, *succ_edge, *e0, *e1;
	edge_node *aet = nullptr, *c_heap = nullptr, *s_heap = nullptr;
	lmt_node *lmt = nullptr, *local_min;
	polygon_node *out_poly = nullptr, *p, *q, *poly, *npoly, *cf = nullptr;
	vertex_node *vtx, *nv;
	h_state horiz[2];
	int in[2], exists[2], parity[2] = {LEFT, LEFT};
	int c, v, contributing, search, scanbeam = 0, sbt_entries = 0;
	int vclass, bl, br, tl, tr;
	double *sbt = nullptr, xb, px, yb, yt, dy, ix, iy;

	/* Test for trivial nullptr result cases */
	if (((subj->num_contours == 0) && (clip->num_contours == 0)) || ((subj->num_contours == 0) && ((op == GPC_INT) || (op == GPC_DIFF))) || ((clip->num_contours == 0) && (op == GPC_INT)))
	{
		result->num_contours = 0;
		result->hole = nullptr;
		result->contour = nullptr;
		return;
	}

	/* Identify potentialy contributing contours */
	if (((op == GPC_INT) || (op == GPC_DIFF)) && (subj->num_contours > 0) && (clip->num_contours > 0))
		minimax_test(subj, clip, op);

	/* Build LMT */
	if (subj->num_contours > 0)
		s_heap = build_lmt(&lmt, &sbtree, &sbt_entries, subj, SUBJ, op);
	if (clip->num_contours > 0)
		c_heap = build_lmt(&lmt, &sbtree, &sbt_entries, clip, CLIP, op);

	/* Return a nullptr result if no contours contribute */
	if (lmt == nullptr)
	{
		result->num_contours = 0;
		result->hole = nullptr;
		result->contour = nullptr;
		reset_lmt(&lmt);
		FREE(s_heap);
		FREE(c_heap);
		return;
	}

	/* Build scanbeam table from scanbeam tree */
	MALLOCDOU(sbt, sbt_entries * sizeof(double), "sbt creation");
	build_sbt(&scanbeam, sbt, sbtree);
	scanbeam = 0;
	free_sbtree(&sbtree);

	/* Allow pointer re-use without causing memory leak */
	if (subj == result)
		gpc_free_polygon(subj);
	if (clip == result)
		gpc_free_polygon(clip);

	/* Invert clip polygon for difference operation */
	if (op == GPC_DIFF)
		parity[CLIP] = RIGHT;

	local_min = lmt;

	/* Process each scanbeam */
	while (scanbeam < sbt_entries)
	{
		/* Set yb and yt to the bottom and top of the scanbeam */
		yb = sbt[scanbeam++];
		if (scanbeam < sbt_entries)
		{
			yt = sbt[scanbeam];
			dy = yt - yb;
		}

		/* === SCANBEAM BOUNDARY PROCESSING ================================ */

		/* If LMT node corresponding to yb exists */
		if (local_min)
		{
			if (local_min->y == yb)
			{
				/* Add edges starting at this local minimum to the AET */
				for (edge = local_min->first_bound; edge; edge = edge->next_bound)
					add_edge_to_aet(&aet, edge, nullptr);

				local_min = local_min->next;
			}
		}

		/* Set dummy previous x value */
		px = -DBL_MAX;

		/* Create bundles within AET */
		e0 = aet;
		e1 = aet;

		/* Set up bundle fields of first edge */
		aet->bundle[ABOVE][aet->type] = (aet->top.y != yb);
		aet->bundle[ABOVE][!aet->type] = FALSE;
		aet->bstate[ABOVE] = UNBUNDLED;

		for (next_edge = aet->next; next_edge; next_edge = next_edge->next)
		{
			/* Set up bundle fields of next edge */
			next_edge->bundle[ABOVE][next_edge->type] = (next_edge->top.y != yb);
			next_edge->bundle[ABOVE][!next_edge->type] = FALSE;
			next_edge->bstate[ABOVE] = UNBUNDLED;

			/* Bundle edges above the scanbeam boundary if they coincide */
			if (next_edge->bundle[ABOVE][next_edge->type])
			{
				if (EQ(e0->xb, next_edge->xb) && EQ(e0->dx, next_edge->dx) && (e0->top.y != yb))
				{
					next_edge->bundle[ABOVE][next_edge->type] ^=
						e0->bundle[ABOVE][next_edge->type];
					next_edge->bundle[ABOVE][!next_edge->type] =
						e0->bundle[ABOVE][!next_edge->type];
					next_edge->bstate[ABOVE] = BUNDLE_HEAD;
					e0->bundle[ABOVE][CLIP] = FALSE;
					e0->bundle[ABOVE][SUBJ] = FALSE;
					e0->bstate[ABOVE] = BUNDLE_TAIL;
				}
				e0 = next_edge;
			}
		}

		horiz[CLIP] = NH;
		horiz[SUBJ] = NH;

		/* Process each edge at this scanbeam boundary */
		for (edge = aet; edge; edge = edge->next)
		{
			exists[CLIP] = edge->bundle[ABOVE][CLIP] +
						   (edge->bundle[BELOW][CLIP] << 1);
			exists[SUBJ] = edge->bundle[ABOVE][SUBJ] +
						   (edge->bundle[BELOW][SUBJ] << 1);

			if (exists[CLIP] || exists[SUBJ])
			{
				/* Set bundle side */
				edge->bside[CLIP] = parity[CLIP];
				edge->bside[SUBJ] = parity[SUBJ];

				/* Determine contributing status and quadrant occupancies */
				switch (op)
				{
				case GPC_DIFF:
				case GPC_INT:
					contributing = (exists[CLIP] && (parity[SUBJ] || horiz[SUBJ])) || (exists[SUBJ] && (parity[CLIP] || horiz[CLIP])) || (exists[CLIP] && exists[SUBJ] && (parity[CLIP] == parity[SUBJ]));
					br = (parity[CLIP]) && (parity[SUBJ]);
					bl = (parity[CLIP] ^ edge->bundle[ABOVE][CLIP]) && (parity[SUBJ] ^ edge->bundle[ABOVE][SUBJ]);
					tr = (parity[CLIP] ^ (horiz[CLIP] != NH)) && (parity[SUBJ] ^ (horiz[SUBJ] != NH));
					tl = (parity[CLIP] ^ (horiz[CLIP] != NH) ^ edge->bundle[BELOW][CLIP]) && (parity[SUBJ] ^ (horiz[SUBJ] != NH) ^ edge->bundle[BELOW][SUBJ]);
					break;
				case GPC_XOR:
					contributing = exists[CLIP] || exists[SUBJ];
					br = (parity[CLIP]) ^ (parity[SUBJ]);
					bl = (parity[CLIP] ^ edge->bundle[ABOVE][CLIP]) ^ (parity[SUBJ] ^ edge->bundle[ABOVE][SUBJ]);
					tr = (parity[CLIP] ^ (horiz[CLIP] != NH)) ^ (parity[SUBJ] ^ (horiz[SUBJ] != NH));
					tl = (parity[CLIP] ^ (horiz[CLIP] != NH) ^ edge->bundle[BELOW][CLIP]) ^ (parity[SUBJ] ^ (horiz[SUBJ] != NH) ^ edge->bundle[BELOW][SUBJ]);
					break;
				case GPC_UNION:
					contributing = (exists[CLIP] && (!parity[SUBJ] || horiz[SUBJ])) || (exists[SUBJ] && (!parity[CLIP] || horiz[CLIP])) || (exists[CLIP] && exists[SUBJ] && (parity[CLIP] == parity[SUBJ]));
					br = (parity[CLIP]) || (parity[SUBJ]);
					bl = (parity[CLIP] ^ edge->bundle[ABOVE][CLIP]) || (parity[SUBJ] ^ edge->bundle[ABOVE][SUBJ]);
					tr = (parity[CLIP] ^ (horiz[CLIP] != NH)) || (parity[SUBJ] ^ (horiz[SUBJ] != NH));
					tl = (parity[CLIP] ^ (horiz[CLIP] != NH) ^ edge->bundle[BELOW][CLIP]) || (parity[SUBJ] ^ (horiz[SUBJ] != NH) ^ edge->bundle[BELOW][SUBJ]);
					break;
				}

				/* Update parity */
				parity[CLIP] ^= edge->bundle[ABOVE][CLIP];
				parity[SUBJ] ^= edge->bundle[ABOVE][SUBJ];

				/* Update horizontal state */
				if (exists[CLIP])
					horiz[CLIP] =
						next_h_state[horiz[CLIP]]
									[((exists[CLIP] - 1) << 1) + parity[CLIP]];
				if (exists[SUBJ])
					horiz[SUBJ] =
						next_h_state[horiz[SUBJ]]
									[((exists[SUBJ] - 1) << 1) + parity[SUBJ]];

				vclass = tr + (tl << 1) + (br << 2) + (bl << 3);

				if (contributing)
				{
					xb = edge->xb;

					switch (vclass)
					{
					case EMN:
					case IMN:
						add_local_min(&out_poly, edge, xb, yb);
						px = xb;
						cf = edge->outp[ABOVE];
						break;
					case ERI:
						if (xb != px)
						{
							add_right(cf, xb, yb);
							px = xb;
						}
						edge->outp[ABOVE] = cf;
						cf = nullptr;
						break;
					case ELI:
						add_left(edge->outp[BELOW], xb, yb);
						px = xb;
						cf = edge->outp[BELOW];
						break;
					case EMX:
						if (xb != px)
						{
							add_left(cf, xb, yb);
							px = xb;
						}
						merge_right(cf, edge->outp[BELOW], out_poly);
						cf = nullptr;
						break;
					case ILI:
						if (xb != px)
						{
							add_left(cf, xb, yb);
							px = xb;
						}
						edge->outp[ABOVE] = cf;
						cf = nullptr;
						break;
					case IRI:
						add_right(edge->outp[BELOW], xb, yb);
						px = xb;
						cf = edge->outp[BELOW];
						edge->outp[BELOW] = nullptr;
						break;
					case IMX:
						if (xb != px)
						{
							add_right(cf, xb, yb);
							px = xb;
						}
						merge_left(cf, edge->outp[BELOW], out_poly);
						cf = nullptr;
						edge->outp[BELOW] = nullptr;
						break;
					case IMM:
						if (xb != px)
						{
							add_right(cf, xb, yb);
							px = xb;
						}
						merge_left(cf, edge->outp[BELOW], out_poly);
						edge->outp[BELOW] = nullptr;
						add_local_min(&out_poly, edge, xb, yb);
						cf = edge->outp[ABOVE];
						break;
					case EMM:
						if (xb != px)
						{
							add_left(cf, xb, yb);
							px = xb;
						}
						merge_right(cf, edge->outp[BELOW], out_poly);
						edge->outp[BELOW] = nullptr;
						add_local_min(&out_poly, edge, xb, yb);
						cf = edge->outp[ABOVE];
						break;
					case LED:
						if (edge->bot.y == yb)
							add_left(edge->outp[BELOW], xb, yb);
						edge->outp[ABOVE] = edge->outp[BELOW];
						px = xb;
						break;
					case RED:
						if (edge->bot.y == yb)
							add_right(edge->outp[BELOW], xb, yb);
						edge->outp[ABOVE] = edge->outp[BELOW];
						px = xb;
						break;
					default:
						break;
					} /* End of switch */
				}	  /* End of contributing conditional */
			}		  /* End of edge exists conditional */
		}			  /* End of AET loop */

		/* Delete terminating edges from the AET, otherwise compute xt */
		for (edge = aet; edge; edge = edge->next)
		{
			if (edge->top.y == yb)
			{
				prev_edge = edge->prev;
				next_edge = edge->next;
				if (prev_edge)
					prev_edge->next = next_edge;
				else
					aet = next_edge;
				if (next_edge)
					next_edge->prev = prev_edge;

				/* Copy bundle head state to the adjacent tail edge if required */
				if ((edge->bstate[BELOW] == BUNDLE_HEAD) && prev_edge)
				{
					if (prev_edge->bstate[BELOW] == BUNDLE_TAIL)
					{
						prev_edge->outp[BELOW] = edge->outp[BELOW];
						prev_edge->bstate[BELOW] = UNBUNDLED;
						if (prev_edge->prev)
							if (prev_edge->prev->bstate[BELOW] == BUNDLE_TAIL)
								prev_edge->bstate[BELOW] = BUNDLE_HEAD;
					}
				}
			}
			else
			{
				if (edge->top.y == yt)
					edge->xt = edge->top.x;
				else
					edge->xt = edge->bot.x + edge->dx * (yt - edge->bot.y);
			}
		}

		if (scanbeam < sbt_entries)
		{
			/* === SCANBEAM INTERIOR PROCESSING ============================== */

			build_intersection_table(&it, aet, dy);

			/* Process each node in the intersection table */
			for (intersect = it; intersect; intersect = intersect->next)
			{
				e0 = intersect->ie[0];
				e1 = intersect->ie[1];

				/* Only generate output for contributing intersections */
				if ((e0->bundle[ABOVE][CLIP] || e0->bundle[ABOVE][SUBJ]) && (e1->bundle[ABOVE][CLIP] || e1->bundle[ABOVE][SUBJ]))
				{
					p = e0->outp[ABOVE];
					q = e1->outp[ABOVE];
					ix = intersect->point.x;
					iy = intersect->point.y + yb;

					in[CLIP] = (e0->bundle[ABOVE][CLIP] && !e0->bside[CLIP]) || (e1->bundle[ABOVE][CLIP] && e1->bside[CLIP]) || (!e0->bundle[ABOVE][CLIP] && !e1->bundle[ABOVE][CLIP] && e0->bside[CLIP] && e1->bside[CLIP]);
					in[SUBJ] = (e0->bundle[ABOVE][SUBJ] && !e0->bside[SUBJ]) || (e1->bundle[ABOVE][SUBJ] && e1->bside[SUBJ]) || (!e0->bundle[ABOVE][SUBJ] && !e1->bundle[ABOVE][SUBJ] && e0->bside[SUBJ] && e1->bside[SUBJ]);

					/* Determine quadrant occupancies */
					switch (op)
					{
					case GPC_DIFF:
					case GPC_INT:
						tr = (in[CLIP]) && (in[SUBJ]);
						tl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP]) && (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ]);
						br = (in[CLIP] ^ e0->bundle[ABOVE][CLIP]) && (in[SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						bl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP] ^ e0->bundle[ABOVE][CLIP]) && (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						break;
					case GPC_XOR:
						tr = (in[CLIP]) ^ (in[SUBJ]);
						tl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP]) ^ (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ]);
						br = (in[CLIP] ^ e0->bundle[ABOVE][CLIP]) ^ (in[SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						bl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP] ^ e0->bundle[ABOVE][CLIP]) ^ (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						break;
					case GPC_UNION:
						tr = (in[CLIP]) || (in[SUBJ]);
						tl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP]) || (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ]);
						br = (in[CLIP] ^ e0->bundle[ABOVE][CLIP]) || (in[SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						bl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP] ^ e0->bundle[ABOVE][CLIP]) || (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						break;
					}

					vclass = tr + (tl << 1) + (br << 2) + (bl << 3);

					switch (vclass)
					{
					case EMN:
						add_local_min(&out_poly, e0, ix, iy);
						e1->outp[ABOVE] = e0->outp[ABOVE];
						break;
					case ERI:
						if (p)
						{
							add_right(p, ix, iy);
							e1->outp[ABOVE] = p;
							e0->outp[ABOVE] = nullptr;
						}
						break;
					case ELI:
						if (q)
						{
							add_left(q, ix, iy);
							e0->outp[ABOVE] = q;
							e1->outp[ABOVE] = nullptr;
						}
						break;
					case EMX:
						if (p && q)
						{
							add_left(p, ix, iy);
							merge_right(p, q, out_poly);
							e0->outp[ABOVE] = nullptr;
							e1->outp[ABOVE] = nullptr;
						}
						break;
					case IMN:
						add_local_min(&out_poly, e0, ix, iy);
						e1->outp[ABOVE] = e0->outp[ABOVE];
						break;
					case ILI:
						if (p)
						{
							add_left(p, ix, iy);
							e1->outp[ABOVE] = p;
							e0->outp[ABOVE] = nullptr;
						}
						break;
					case IRI:
						if (q)
						{
							add_right(q, ix, iy);
							e0->outp[ABOVE] = q;
							e1->outp[ABOVE] = nullptr;
						}
						break;
					case IMX:
						if (p && q)
						{
							add_right(p, ix, iy);
							merge_left(p, q, out_poly);
							e0->outp[ABOVE] = nullptr;
							e1->outp[ABOVE] = nullptr;
						}
						break;
					case IMM:
						if (p && q)
						{
							add_right(p, ix, iy);
							merge_left(p, q, out_poly);
							add_local_min(&out_poly, e0, ix, iy);
							e1->outp[ABOVE] = e0->outp[ABOVE];
						}
						break;
					case EMM:
						if (p && q)
						{
							add_left(p, ix, iy);
							merge_right(p, q, out_poly);
							add_local_min(&out_poly, e0, ix, iy);
							e1->outp[ABOVE] = e0->outp[ABOVE];
						}
						break;
					default:
						break;
					} /* End of switch */
				}	  /* End of contributing intersection conditional */

				/* Swap bundle sides in response to edge crossing */
				if (e0->bundle[ABOVE][CLIP])
					e1->bside[CLIP] = !e1->bside[CLIP];
				if (e1->bundle[ABOVE][CLIP])
					e0->bside[CLIP] = !e0->bside[CLIP];
				if (e0->bundle[ABOVE][SUBJ])
					e1->bside[SUBJ] = !e1->bside[SUBJ];
				if (e1->bundle[ABOVE][SUBJ])
					e0->bside[SUBJ] = !e0->bside[SUBJ];

				/* Swap e0 and e1 bundles in the AET */
				prev_edge = e0->prev;
				next_edge = e1->next;
				if (next_edge)
					next_edge->prev = e0;

				if (e0->bstate[ABOVE] == BUNDLE_HEAD)
				{
					search = TRUE;
					while (search)
					{
						prev_edge = prev_edge->prev;
						if (prev_edge)
						{
							if (prev_edge->bstate[ABOVE] != BUNDLE_TAIL)
								search = FALSE;
						}
						else
							search = FALSE;
					}
				}
				if (!prev_edge)
				{
					aet->prev = e1;
					e1->next = aet;
					aet = e0->next;
				}
				else
				{
					prev_edge->next->prev = e1;
					e1->next = prev_edge->next;
					prev_edge->next = e0->next;
				}
				e0->next->prev = prev_edge;
				e1->next->prev = e1;
				e0->next = next_edge;
			} /* End of IT loop*/

			/* Prepare for next scanbeam */
			for (edge = aet; edge; edge = next_edge)
			{
				next_edge = edge->next;
				succ_edge = edge->succ;

				if ((edge->top.y == yt) && succ_edge)
				{
					/* Replace AET edge by its successor */
					succ_edge->outp[BELOW] = edge->outp[ABOVE];
					succ_edge->bstate[BELOW] = edge->bstate[ABOVE];
					succ_edge->bundle[BELOW][CLIP] = edge->bundle[ABOVE][CLIP];
					succ_edge->bundle[BELOW][SUBJ] = edge->bundle[ABOVE][SUBJ];
					prev_edge = edge->prev;
					if (prev_edge)
						prev_edge->next = succ_edge;
					else
						aet = succ_edge;
					if (next_edge)
						next_edge->prev = succ_edge;
					succ_edge->prev = prev_edge;
					succ_edge->next = next_edge;
				}
				else
				{
					/* Update this edge */
					edge->outp[BELOW] = edge->outp[ABOVE];
					edge->bstate[BELOW] = edge->bstate[ABOVE];
					edge->bundle[BELOW][CLIP] = edge->bundle[ABOVE][CLIP];
					edge->bundle[BELOW][SUBJ] = edge->bundle[ABOVE][SUBJ];
					edge->xb = edge->xt;
				}
				edge->outp[ABOVE] = nullptr;
			}
		}
	} /* === END OF SCANBEAM PROCESSING ================================== */

	/* Generate result polygon from out_poly */
	result->contour = nullptr;
	result->hole = nullptr;
	result->num_contours = count_contours(out_poly);
	if (result->num_contours > 0)
	{
		MALLOCINT(result->hole, result->num_contours * sizeof(int), "hole flag table creation");
		MALLOCGPC(result->contour, result->num_contours * sizeof(gpc_vertex_list), "contour creation");

		c = 0;
		for (poly = out_poly; poly; poly = npoly)
		{
			npoly = poly->next;
			if (poly->active)
			{
				result->hole[c] = poly->proxy->hole;
				result->contour[c].num_vertices = poly->active;
				MALLOCGPCV(result->contour[c].vertex,
						   result->contour[c].num_vertices * sizeof(gpc_vertex),
						   "vertex creation");

				v = result->contour[c].num_vertices - 1;
				for (vtx = poly->proxy->v[LEFT]; vtx; vtx = nv)
				{
					nv = vtx->next;
					result->contour[c].vertex[v].x = vtx->x;
					result->contour[c].vertex[v].y = vtx->y;
					FREE(vtx);
					v--;
				}
				c++;
			}
			FREE(poly);
		}
	}

	/* Tidy up */
	reset_it(&it);
	reset_lmt(&lmt);
	FREE(c_heap);
	FREE(s_heap);
	FREE(sbt);
}

void gpc::gpc_free_tristrip(gpc_tristrip *t)
{
	int s;

	for (s = 0; s < t->num_strips; s++)
		FREE(t->strip[s].vertex);
	FREE(t->strip);
	t->num_strips = 0;
}

void gpc::gpc_polygon_to_tristrip(gpc_polygon *s, gpc_tristrip *t)
{
	gpc_polygon c;

	c.num_contours = 0;
	c.hole = nullptr;
	c.contour = nullptr;
	gpc_tristrip_clip(GPC_DIFF, s, &c, t);
}

void gpc::gpc_tristrip_clip(gpc_op op, gpc_polygon *subj, gpc_polygon *clip,
							gpc_tristrip *result)
{
	sb_tree *sbtree = nullptr;
	it_node *it = nullptr, *intersect;
	edge_node *edge, *prev_edge, *next_edge, *succ_edge, *e0, *e1;
	edge_node *aet = nullptr, *c_heap = nullptr, *s_heap = nullptr, *cf;
	lmt_node *lmt = nullptr, *local_min;
	polygon_node *tlist = nullptr, *tn, *tnn, *p, *q;
	vertex_node *lt, *ltn, *rt, *rtn;
	h_state horiz[2];
	vertex_type cft;
	int in[2], exists[2], parity[2] = {LEFT, LEFT};
	int s, v, contributing, search, scanbeam = 0, sbt_entries = 0;
	int vclass, bl, br, tl, tr;
	double *sbt = nullptr, xb, px, nx, yb, yt, dy, ix, iy;

	/* Test for trivial nullptr result cases */
	if (((subj->num_contours == 0) && (clip->num_contours == 0)) || ((subj->num_contours == 0) && ((op == GPC_INT) || (op == GPC_DIFF))) || ((clip->num_contours == 0) && (op == GPC_INT)))
	{
		result->num_strips = 0;
		result->strip = nullptr;
		return;
	}

	/* Identify potentialy contributing contours */
	if (((op == GPC_INT) || (op == GPC_DIFF)) && (subj->num_contours > 0) && (clip->num_contours > 0))
		minimax_test(subj, clip, op);

	/* Build LMT */
	if (subj->num_contours > 0)
		s_heap = build_lmt(&lmt, &sbtree, &sbt_entries, subj, SUBJ, op);
	if (clip->num_contours > 0)
		c_heap = build_lmt(&lmt, &sbtree, &sbt_entries, clip, CLIP, op);

	/* Return a nullptr result if no contours contribute */
	if (lmt == nullptr)
	{
		result->num_strips = 0;
		result->strip = nullptr;
		reset_lmt(&lmt);
		FREE(s_heap);
		FREE(c_heap);
		return;
	}

	/* Build scanbeam table from scanbeam tree */
	MALLOCDOU(sbt, sbt_entries * sizeof(double), "sbt creation");
	build_sbt(&scanbeam, sbt, sbtree);
	scanbeam = 0;
	free_sbtree(&sbtree);

	/* Invert clip polygon for difference operation */
	if (op == GPC_DIFF)
		parity[CLIP] = RIGHT;

	local_min = lmt;

	/* Process each scanbeam */
	while (scanbeam < sbt_entries)
	{
		/* Set yb and yt to the bottom and top of the scanbeam */
		yb = sbt[scanbeam++];
		if (scanbeam < sbt_entries)
		{
			yt = sbt[scanbeam];
			dy = yt - yb;
		}

		/* === SCANBEAM BOUNDARY PROCESSING ================================ */

		/* If LMT node corresponding to yb exists */
		if (local_min)
		{
			if (local_min->y == yb)
			{
				/* Add edges starting at this local minimum to the AET */
				for (edge = local_min->first_bound; edge; edge = edge->next_bound)
					add_edge_to_aet(&aet, edge, nullptr);

				local_min = local_min->next;
			}
		}

		/* Set dummy previous x value */
		px = -DBL_MAX;

		/* Create bundles within AET */
		e0 = aet;
		e1 = aet;

		/* Set up bundle fields of first edge */
		aet->bundle[ABOVE][aet->type] = (aet->top.y != yb);
		aet->bundle[ABOVE][!aet->type] = FALSE;
		aet->bstate[ABOVE] = UNBUNDLED;

		for (next_edge = aet->next; next_edge; next_edge = next_edge->next)
		{
			/* Set up bundle fields of next edge */
			next_edge->bundle[ABOVE][next_edge->type] = (next_edge->top.y != yb);
			next_edge->bundle[ABOVE][!next_edge->type] = FALSE;
			next_edge->bstate[ABOVE] = UNBUNDLED;

			/* Bundle edges above the scanbeam boundary if they coincide */
			if (next_edge->bundle[ABOVE][next_edge->type])
			{
				if (EQ(e0->xb, next_edge->xb) && EQ(e0->dx, next_edge->dx) && (e0->top.y != yb))
				{
					next_edge->bundle[ABOVE][next_edge->type] ^=
						e0->bundle[ABOVE][next_edge->type];
					next_edge->bundle[ABOVE][!next_edge->type] =
						e0->bundle[ABOVE][!next_edge->type];
					next_edge->bstate[ABOVE] = BUNDLE_HEAD;
					e0->bundle[ABOVE][CLIP] = FALSE;
					e0->bundle[ABOVE][SUBJ] = FALSE;
					e0->bstate[ABOVE] = BUNDLE_TAIL;
				}
				e0 = next_edge;
			}
		}

		horiz[CLIP] = NH;
		horiz[SUBJ] = NH;

		/* Process each edge at this scanbeam boundary */
		for (edge = aet; edge; edge = edge->next)
		{
			exists[CLIP] = edge->bundle[ABOVE][CLIP] +
						   (edge->bundle[BELOW][CLIP] << 1);
			exists[SUBJ] = edge->bundle[ABOVE][SUBJ] +
						   (edge->bundle[BELOW][SUBJ] << 1);

			if (exists[CLIP] || exists[SUBJ])
			{
				/* Set bundle side */
				edge->bside[CLIP] = parity[CLIP];
				edge->bside[SUBJ] = parity[SUBJ];

				/* Determine contributing status and quadrant occupancies */
				switch (op)
				{
				case GPC_DIFF:
				case GPC_INT:
					contributing = (exists[CLIP] && (parity[SUBJ] || horiz[SUBJ])) || (exists[SUBJ] && (parity[CLIP] || horiz[CLIP])) || (exists[CLIP] && exists[SUBJ] && (parity[CLIP] == parity[SUBJ]));
					br = (parity[CLIP]) && (parity[SUBJ]);
					bl = (parity[CLIP] ^ edge->bundle[ABOVE][CLIP]) && (parity[SUBJ] ^ edge->bundle[ABOVE][SUBJ]);
					tr = (parity[CLIP] ^ (horiz[CLIP] != NH)) && (parity[SUBJ] ^ (horiz[SUBJ] != NH));
					tl = (parity[CLIP] ^ (horiz[CLIP] != NH) ^ edge->bundle[BELOW][CLIP]) && (parity[SUBJ] ^ (horiz[SUBJ] != NH) ^ edge->bundle[BELOW][SUBJ]);
					break;
				case GPC_XOR:
					contributing = exists[CLIP] || exists[SUBJ];
					br = (parity[CLIP]) ^ (parity[SUBJ]);
					bl = (parity[CLIP] ^ edge->bundle[ABOVE][CLIP]) ^ (parity[SUBJ] ^ edge->bundle[ABOVE][SUBJ]);
					tr = (parity[CLIP] ^ (horiz[CLIP] != NH)) ^ (parity[SUBJ] ^ (horiz[SUBJ] != NH));
					tl = (parity[CLIP] ^ (horiz[CLIP] != NH) ^ edge->bundle[BELOW][CLIP]) ^ (parity[SUBJ] ^ (horiz[SUBJ] != NH) ^ edge->bundle[BELOW][SUBJ]);
					break;
				case GPC_UNION:
					contributing = (exists[CLIP] && (!parity[SUBJ] || horiz[SUBJ])) || (exists[SUBJ] && (!parity[CLIP] || horiz[CLIP])) || (exists[CLIP] && exists[SUBJ] && (parity[CLIP] == parity[SUBJ]));
					br = (parity[CLIP]) || (parity[SUBJ]);
					bl = (parity[CLIP] ^ edge->bundle[ABOVE][CLIP]) || (parity[SUBJ] ^ edge->bundle[ABOVE][SUBJ]);
					tr = (parity[CLIP] ^ (horiz[CLIP] != NH)) || (parity[SUBJ] ^ (horiz[SUBJ] != NH));
					tl = (parity[CLIP] ^ (horiz[CLIP] != NH) ^ edge->bundle[BELOW][CLIP]) || (parity[SUBJ] ^ (horiz[SUBJ] != NH) ^ edge->bundle[BELOW][SUBJ]);
					break;
				}

				/* Update parity */
				parity[CLIP] ^= edge->bundle[ABOVE][CLIP];
				parity[SUBJ] ^= edge->bundle[ABOVE][SUBJ];

				/* Update horizontal state */
				if (exists[CLIP])
					horiz[CLIP] =
						next_h_state[horiz[CLIP]]
									[((exists[CLIP] - 1) << 1) + parity[CLIP]];
				if (exists[SUBJ])
					horiz[SUBJ] =
						next_h_state[horiz[SUBJ]]
									[((exists[SUBJ] - 1) << 1) + parity[SUBJ]];

				vclass = tr + (tl << 1) + (br << 2) + (bl << 3);

				if (contributing)
				{
					xb = edge->xb;

					switch (vclass)
					{
					case EMN:
						new_tristrip(&tlist, edge, xb, yb);
						cf = edge;
						break;
					case ERI:
						edge->outp[ABOVE] = cf->outp[ABOVE];
						if (xb != cf->xb)
							VERTEX(edge, ABOVE, RIGHT, xb, yb);
						cf = nullptr;
						break;
					case ELI:
						VERTEX(edge, BELOW, LEFT, xb, yb);
						edge->outp[ABOVE] = nullptr;
						cf = edge;
						break;
					case EMX:
						if (xb != cf->xb)
							VERTEX(edge, BELOW, RIGHT, xb, yb);
						edge->outp[ABOVE] = nullptr;
						cf = nullptr;
						break;
					case IMN:
						if (cft == LED)
						{
							if (cf->bot.y != yb)
								VERTEX(cf, BELOW, LEFT, cf->xb, yb);
							new_tristrip(&tlist, cf, cf->xb, yb);
						}
						edge->outp[ABOVE] = cf->outp[ABOVE];
						VERTEX(edge, ABOVE, RIGHT, xb, yb);
						break;
					case ILI:
						new_tristrip(&tlist, edge, xb, yb);
						cf = edge;
						cft = ILI;
						break;
					case IRI:
						if (cft == LED)
						{
							if (cf->bot.y != yb)
								VERTEX(cf, BELOW, LEFT, cf->xb, yb);
							new_tristrip(&tlist, cf, cf->xb, yb);
						}
						VERTEX(edge, BELOW, RIGHT, xb, yb);
						edge->outp[ABOVE] = nullptr;
						break;
					case IMX:
						VERTEX(edge, BELOW, LEFT, xb, yb);
						edge->outp[ABOVE] = nullptr;
						cft = IMX;
						break;
					case IMM:
						VERTEX(edge, BELOW, LEFT, xb, yb);
						edge->outp[ABOVE] = cf->outp[ABOVE];
						if (xb != cf->xb)
							VERTEX(cf, ABOVE, RIGHT, xb, yb);
						cf = edge;
						break;
					case EMM:
						VERTEX(edge, BELOW, RIGHT, xb, yb);
						edge->outp[ABOVE] = nullptr;
						new_tristrip(&tlist, edge, xb, yb);
						cf = edge;
						break;
					case LED:
						if (edge->bot.y == yb)
							VERTEX(edge, BELOW, LEFT, xb, yb);
						edge->outp[ABOVE] = edge->outp[BELOW];
						cf = edge;
						cft = LED;
						break;
					case RED:
						edge->outp[ABOVE] = cf->outp[ABOVE];
						if (cft == LED)
						{
							if (cf->bot.y == yb)
							{
								VERTEX(edge, BELOW, RIGHT, xb, yb);
							}
							else
							{
								if (edge->bot.y == yb)
								{
									VERTEX(cf, BELOW, LEFT, cf->xb, yb);
									VERTEX(edge, BELOW, RIGHT, xb, yb);
								}
							}
						}
						else
						{
							VERTEX(edge, BELOW, RIGHT, xb, yb);
							VERTEX(edge, ABOVE, RIGHT, xb, yb);
						}
						cf = nullptr;
						break;
					default:
						break;
					} /* End of switch */
				}	  /* End of contributing conditional */
			}		  /* End of edge exists conditional */
		}			  /* End of AET loop */

		/* Delete terminating edges from the AET, otherwise compute xt */
		for (edge = aet; edge; edge = edge->next)
		{
			if (edge->top.y == yb)
			{
				prev_edge = edge->prev;
				next_edge = edge->next;
				if (prev_edge)
					prev_edge->next = next_edge;
				else
					aet = next_edge;
				if (next_edge)
					next_edge->prev = prev_edge;

				/* Copy bundle head state to the adjacent tail edge if required */
				if ((edge->bstate[BELOW] == BUNDLE_HEAD) && prev_edge)
				{
					if (prev_edge->bstate[BELOW] == BUNDLE_TAIL)
					{
						prev_edge->outp[BELOW] = edge->outp[BELOW];
						prev_edge->bstate[BELOW] = UNBUNDLED;
						if (prev_edge->prev)
							if (prev_edge->prev->bstate[BELOW] == BUNDLE_TAIL)
								prev_edge->bstate[BELOW] = BUNDLE_HEAD;
					}
				}
			}
			else
			{
				if (edge->top.y == yt)
					edge->xt = edge->top.x;
				else
					edge->xt = edge->bot.x + edge->dx * (yt - edge->bot.y);
			}
		}

		if (scanbeam < sbt_entries)
		{
			/* === SCANBEAM INTERIOR PROCESSING ============================== */

			build_intersection_table(&it, aet, dy);

			/* Process each node in the intersection table */
			for (intersect = it; intersect; intersect = intersect->next)
			{
				e0 = intersect->ie[0];
				e1 = intersect->ie[1];

				/* Only generate output for contributing intersections */
				if ((e0->bundle[ABOVE][CLIP] || e0->bundle[ABOVE][SUBJ]) && (e1->bundle[ABOVE][CLIP] || e1->bundle[ABOVE][SUBJ]))
				{
					p = e0->outp[ABOVE];
					q = e1->outp[ABOVE];
					ix = intersect->point.x;
					iy = intersect->point.y + yb;

					in[CLIP] = (e0->bundle[ABOVE][CLIP] && !e0->bside[CLIP]) || (e1->bundle[ABOVE][CLIP] && e1->bside[CLIP]) || (!e0->bundle[ABOVE][CLIP] && !e1->bundle[ABOVE][CLIP] && e0->bside[CLIP] && e1->bside[CLIP]);
					in[SUBJ] = (e0->bundle[ABOVE][SUBJ] && !e0->bside[SUBJ]) || (e1->bundle[ABOVE][SUBJ] && e1->bside[SUBJ]) || (!e0->bundle[ABOVE][SUBJ] && !e1->bundle[ABOVE][SUBJ] && e0->bside[SUBJ] && e1->bside[SUBJ]);

					/* Determine quadrant occupancies */
					switch (op)
					{
					case GPC_DIFF:
					case GPC_INT:
						tr = (in[CLIP]) && (in[SUBJ]);
						tl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP]) && (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ]);
						br = (in[CLIP] ^ e0->bundle[ABOVE][CLIP]) && (in[SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						bl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP] ^ e0->bundle[ABOVE][CLIP]) && (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						break;
					case GPC_XOR:
						tr = (in[CLIP]) ^ (in[SUBJ]);
						tl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP]) ^ (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ]);
						br = (in[CLIP] ^ e0->bundle[ABOVE][CLIP]) ^ (in[SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						bl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP] ^ e0->bundle[ABOVE][CLIP]) ^ (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						break;
					case GPC_UNION:
						tr = (in[CLIP]) || (in[SUBJ]);
						tl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP]) || (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ]);
						br = (in[CLIP] ^ e0->bundle[ABOVE][CLIP]) || (in[SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						bl = (in[CLIP] ^ e1->bundle[ABOVE][CLIP] ^ e0->bundle[ABOVE][CLIP]) || (in[SUBJ] ^ e1->bundle[ABOVE][SUBJ] ^ e0->bundle[ABOVE][SUBJ]);
						break;
					}

					vclass = tr + (tl << 1) + (br << 2) + (bl << 3);

					switch (vclass)
					{
					case EMN:
						new_tristrip(&tlist, e1, ix, iy);
						e0->outp[ABOVE] = e1->outp[ABOVE];
						break;
					case ERI:
						if (p)
						{
							P_EDGE(prev_edge, e0, ABOVE, px, iy);
							VERTEX(prev_edge, ABOVE, LEFT, px, iy);
							VERTEX(e0, ABOVE, RIGHT, ix, iy);
							e1->outp[ABOVE] = e0->outp[ABOVE];
							e0->outp[ABOVE] = nullptr;
						}
						break;
					case ELI:
						if (q)
						{
							N_EDGE(next_edge, e1, ABOVE, nx, iy);
							VERTEX(e1, ABOVE, LEFT, ix, iy);
							VERTEX(next_edge, ABOVE, RIGHT, nx, iy);
							e0->outp[ABOVE] = e1->outp[ABOVE];
							e1->outp[ABOVE] = nullptr;
						}
						break;
					case EMX:
						if (p && q)
						{
							VERTEX(e0, ABOVE, LEFT, ix, iy);
							e0->outp[ABOVE] = nullptr;
							e1->outp[ABOVE] = nullptr;
						}
						break;
					case IMN:
						P_EDGE(prev_edge, e0, ABOVE, px, iy);
						VERTEX(prev_edge, ABOVE, LEFT, px, iy);
						N_EDGE(next_edge, e1, ABOVE, nx, iy);
						VERTEX(next_edge, ABOVE, RIGHT, nx, iy);
						new_tristrip(&tlist, prev_edge, px, iy);
						e1->outp[ABOVE] = prev_edge->outp[ABOVE];
						VERTEX(e1, ABOVE, RIGHT, ix, iy);
						new_tristrip(&tlist, e0, ix, iy);
						next_edge->outp[ABOVE] = e0->outp[ABOVE];
						VERTEX(next_edge, ABOVE, RIGHT, nx, iy);
						break;
					case ILI:
						if (p)
						{
							VERTEX(e0, ABOVE, LEFT, ix, iy);
							N_EDGE(next_edge, e1, ABOVE, nx, iy);
							VERTEX(next_edge, ABOVE, RIGHT, nx, iy);
							e1->outp[ABOVE] = e0->outp[ABOVE];
							e0->outp[ABOVE] = nullptr;
						}
						break;
					case IRI:
						if (q)
						{
							VERTEX(e1, ABOVE, RIGHT, ix, iy);
							P_EDGE(prev_edge, e0, ABOVE, px, iy);
							VERTEX(prev_edge, ABOVE, LEFT, px, iy);
							e0->outp[ABOVE] = e1->outp[ABOVE];
							e1->outp[ABOVE] = nullptr;
						}
						break;
					case IMX:
						if (p && q)
						{
							VERTEX(e0, ABOVE, RIGHT, ix, iy);
							VERTEX(e1, ABOVE, LEFT, ix, iy);
							e0->outp[ABOVE] = nullptr;
							e1->outp[ABOVE] = nullptr;
							P_EDGE(prev_edge, e0, ABOVE, px, iy);
							VERTEX(prev_edge, ABOVE, LEFT, px, iy);
							new_tristrip(&tlist, prev_edge, px, iy);
							N_EDGE(next_edge, e1, ABOVE, nx, iy);
							VERTEX(next_edge, ABOVE, RIGHT, nx, iy);
							next_edge->outp[ABOVE] = prev_edge->outp[ABOVE];
							VERTEX(next_edge, ABOVE, RIGHT, nx, iy);
						}
						break;
					case IMM:
						if (p && q)
						{
							VERTEX(e0, ABOVE, RIGHT, ix, iy);
							VERTEX(e1, ABOVE, LEFT, ix, iy);
							P_EDGE(prev_edge, e0, ABOVE, px, iy);
							VERTEX(prev_edge, ABOVE, LEFT, px, iy);
							new_tristrip(&tlist, prev_edge, px, iy);
							N_EDGE(next_edge, e1, ABOVE, nx, iy);
							VERTEX(next_edge, ABOVE, RIGHT, nx, iy);
							e1->outp[ABOVE] = prev_edge->outp[ABOVE];
							VERTEX(e1, ABOVE, RIGHT, ix, iy);
							new_tristrip(&tlist, e0, ix, iy);
							next_edge->outp[ABOVE] = e0->outp[ABOVE];
							VERTEX(next_edge, ABOVE, RIGHT, nx, iy);
						}
						break;
					case EMM:
						if (p && q)
						{
							VERTEX(e0, ABOVE, LEFT, ix, iy);
							new_tristrip(&tlist, e1, ix, iy);
							e0->outp[ABOVE] = e1->outp[ABOVE];
						}
						break;
					default:
						break;
					} /* End of switch */
				}	  /* End of contributing intersection conditional */

				/* Swap bundle sides in response to edge crossing */
				if (e0->bundle[ABOVE][CLIP])
					e1->bside[CLIP] = !e1->bside[CLIP];
				if (e1->bundle[ABOVE][CLIP])
					e0->bside[CLIP] = !e0->bside[CLIP];
				if (e0->bundle[ABOVE][SUBJ])
					e1->bside[SUBJ] = !e1->bside[SUBJ];
				if (e1->bundle[ABOVE][SUBJ])
					e0->bside[SUBJ] = !e0->bside[SUBJ];

				/* Swap e0 and e1 bundles in the AET */
				prev_edge = e0->prev;
				next_edge = e1->next;
				if (e1->next)
					e1->next->prev = e0;

				if (e0->bstate[ABOVE] == BUNDLE_HEAD)
				{
					search = TRUE;
					while (search)
					{
						prev_edge = prev_edge->prev;
						if (prev_edge)
						{
							if (prev_edge->bundle[ABOVE][CLIP] || prev_edge->bundle[ABOVE][SUBJ] || (prev_edge->bstate[ABOVE] == BUNDLE_HEAD))
								search = FALSE;
						}
						else
							search = FALSE;
					}
				}
				if (!prev_edge)
				{
					e1->next = aet;
					aet = e0->next;
				}
				else
				{
					e1->next = prev_edge->next;
					prev_edge->next = e0->next;
				}
				e0->next->prev = prev_edge;
				e1->next->prev = e1;
				e0->next = next_edge;
			} /* End of IT loop*/

			/* Prepare for next scanbeam */
			for (edge = aet; edge; edge = next_edge)
			{
				next_edge = edge->next;
				succ_edge = edge->succ;

				if ((edge->top.y == yt) && succ_edge)
				{
					/* Replace AET edge by its successor */
					succ_edge->outp[BELOW] = edge->outp[ABOVE];
					succ_edge->bstate[BELOW] = edge->bstate[ABOVE];
					succ_edge->bundle[BELOW][CLIP] = edge->bundle[ABOVE][CLIP];
					succ_edge->bundle[BELOW][SUBJ] = edge->bundle[ABOVE][SUBJ];
					prev_edge = edge->prev;
					if (prev_edge)
						prev_edge->next = succ_edge;
					else
						aet = succ_edge;
					if (next_edge)
						next_edge->prev = succ_edge;
					succ_edge->prev = prev_edge;
					succ_edge->next = next_edge;
				}
				else
				{
					/* Update this edge */
					edge->outp[BELOW] = edge->outp[ABOVE];
					edge->bstate[BELOW] = edge->bstate[ABOVE];
					edge->bundle[BELOW][CLIP] = edge->bundle[ABOVE][CLIP];
					edge->bundle[BELOW][SUBJ] = edge->bundle[ABOVE][SUBJ];
					edge->xb = edge->xt;
				}
				edge->outp[ABOVE] = nullptr;
			}
		}
	} /* === END OF SCANBEAM PROCESSING ================================== */

	/* Generate result tristrip from tlist */
	result->strip = nullptr;
	result->num_strips = count_tristrips(tlist);
	if (result->num_strips > 0)
	{
		MALLOCGPC(result->strip, result->num_strips * sizeof(gpc_vertex_list),
				  "tristrip list creation");

		s = 0;
		for (tn = tlist; tn; tn = tnn)
		{
			tnn = tn->next;

			if (tn->active > 2)
			{
				/* Valid tristrip: copy the vertices and free the heap */
				result->strip[s].num_vertices = tn->active;
				MALLOCGPCV(result->strip[s].vertex, tn->active * sizeof(gpc_vertex),
						   "tristrip creation");
				v = 0;
				if (INVERT_TRISTRIPS)
				{
					lt = tn->v[RIGHT];
					rt = tn->v[LEFT];
				}
				else
				{
					lt = tn->v[LEFT];
					rt = tn->v[RIGHT];
				}
				while (lt || rt)
				{
					if (lt)
					{
						ltn = lt->next;
						result->strip[s].vertex[v].x = lt->x;
						result->strip[s].vertex[v].y = lt->y;
						v++;
						FREE(lt);
						lt = ltn;
					}
					if (rt)
					{
						rtn = rt->next;
						result->strip[s].vertex[v].x = rt->x;
						result->strip[s].vertex[v].y = rt->y;
						v++;
						FREE(rt);
						rt = rtn;
					}
				}
				s++;
			}
			else
			{
				/* Invalid tristrip: just free the heap */
				for (lt = tn->v[LEFT]; lt; lt = ltn)
				{
					ltn = lt->next;
					FREE(lt);
				}
				for (rt = tn->v[RIGHT]; rt; rt = rtn)
				{
					rtn = rt->next;
					FREE(rt);
				}
			}
			FREE(tn);
		}
	}

	/* Tidy up */
	reset_it(&it);
	reset_lmt(&lmt);
	FREE(c_heap);
	FREE(s_heap);
	FREE(sbt);
}

/*
int notreject(denom,num,tl,tu)
double denom,num,*tl,*tu;
{
double t;
int accept;
accept=true;
if(denom<0)
{
t=num/denom;
if(t>*tu)accept=FALSE;
else if(t>*tl)*tl=t;
}
else if(denom>0)
{
t=num/denom;
if(t<*tl)accept=FALSE;
else if(t<*tu)*tu=t;
}
else if(num<0)accept=FALSE;
return(accept);
}

  L_B_LineClip(p1,p2,tmin,tmax,visible)
  float p1[2],p2[2],*tmin,*tmax;
  {
  *visible=FALSE;
  tl=0.0;tu=1.0;
  dx=p2[0]-p1[0];
  dy=p2[1]-p1[1];
  if(notreject(-dx,x1-xl,&tl,&tu))
		if(notreject(+dx,xr-x1,&tl,&tu))
		if(notreject(-dy,y-yb,&tl,&tu))
		if(notreject(dy,yt-y1,&tl,&tu))
		{
		*visible=TRUE;
		*tmin=tl;
		*tmax=tu;
		}
		}
		*/

/*
void gpc::gpc_polyline_clip(gpc_op op, gpc_polygon *subj, gpc_polygon *clip,
gpc_polygon *result)
{
int flag,c,v,cp;
double x,y,xc,yc,xd,yd,xt,yt;
gpc_polygon *p;
double *map_p;
int pp;
double x0,y0,yj,xj;

  pp=1;
  map_p=(double *)malloc(60000*sizeof(double));
  map_p[0]=0;

	p=clip;	//取出裁剪多边形

	  xc=p->contour[0].vertex[0].x;
	  yc=p->contour[0].vertex[0].y;
	  xd=p->contour[0].vertex[2].x;
	  yd=p->contour[0].vertex[2].y;
	  cp=0;
	  p=subj;//取出被裁剪的折线段
	  for (c= 0; c < p->num_contours; c++)
	  {
	  //取出第一点
	  x0=x=p->contour[c].vertex[0].x;
	  y0=y=p->contour[c].vertex[0].y;
	  //判断第一点(x,y)是否在矩形内
	  flag=inBox(x,y,xc,yc,xd,yd);
	  if(flag==1){//记录该点，并开始一条折线
	  map_p[0]++;
	  pp=pp+cp;
	  cp=1;
	  map_p[pp]=1;
	  map_p[pp+cp]=x;
	  map_p[pp+cp+1]=y;
	  cp+=2;
	  }
	  //取下一点
	  for (v = 1; v < p->contour[c].num_vertices; v++){
	  x=p->contour[c].vertex[v].x;
	  y=p->contour[c].vertex[v].y;
	  //判断当前点(x,y)是否在矩形内
	  if(inBox(x,y,xc,yc,xd,yd)){	//在
	  if(flag){//判断上一点是否在矩形内
	  //在,记录该点，并继续一条折线
	  map_p[pp]++;
	  map_p[pp+cp]=x;
	  map_p[pp+cp+1]=y;
	  x0=x;
	  y0=y;
	  cp+=2;
	  }
	  else{//不在,求交点，并开始一条折线(起始点为交点,有两个点)
	  if(calc_intersection(x0,y0,x,y,xc,yc,xc,yd,&xj,&yj)
	  || calc_intersection(x0,y0,x,y,xc,yc,xd,yc,&xj,&yj)
	  || calc_intersection(x0,y0,x,y,xd,yc,xd,yd,&xj,&yj)
	  || calc_intersection(x0,y0,x,y,xc,yd,xd,yd,&xj,&yj))
	  {
	  map_p[0]++;
	  pp=pp+cp;
	  cp=1;
	  map_p[pp]=2;
	  map_p[pp+cp]=xj;
	  map_p[pp+cp+1]=yj;
	  map_p[pp+cp+2]=x;
	  map_p[pp+cp+3]=y;
	  cp+=4;
	  }
	  x0=x;y0=y;
	  }
	  flag=1;
	  }
	  else{		//不在
	  if(flag){//判断上一点是否在矩形内
	  //如果在则求交点，并结束本条折线
	  if(calc_intersection(x0,y0,x,y,xc,yc,xc,yd,&xj,&yj)
	  || calc_intersection(x0,y0,x,y,xc,yc,xd,yc,&xj,&yj)
	  || calc_intersection(x0,y0,x,y,xd,yc,xd,yd,&xj,&yj)
	  || calc_intersection(x0,y0,x,y,xc,yd,xd,yd,&xj,&yj))
	  {
	  map_p[pp]++;
	  map_p[pp+cp]=xj;
	  map_p[pp+cp+1]=yj;
	  x0=xj;y0=yj;
	  cp+=2;
	  }
	  else{
	  x0=x;y0=y;
	  //不可能
	  }
	  }
	  else{//如果不在则判断是否存在两个交点，如果存在则生成一条由两个点组成的折线
	  if(calc_intersection(x0,y0,x,y,xc,yc,xc,yd,&xj,&yj)){
	  xt=xj;yt=yj;
	  if(calc_intersection(x0,y0,x,y,xc,yc,xd,yc,&xj,&yj)
	  || calc_intersection(x0,y0,x,y,xd,yc,xd,yd,&xj,&yj)
	  || calc_intersection(x0,y0,x,y,xc,yd,xd,yd,&xj,&yj)){

		map_p[0]++;
		pp=pp+cp;
		cp=1;
		map_p[pp]=2;
		map_p[pp+cp]=xt;
		map_p[pp+cp+1]=yt;
		map_p[pp+cp+2]=xj;
		map_p[pp+cp+3]=yj;
		cp+=4;
		}
		}
		else{
		if(calc_intersection(x0,y0,x,y,xc,yc,xd,yc,&xj,&yj)){
		xt=xj;yt=yj;
		if(calc_intersection(x0,y0,x,y,xd,yc,xd,yd,&xj,&yj)
		|| calc_intersection(x0,y0,x,y,xc,yd,xd,yd,&xj,&yj)){

		  map_p[0]++;
		  pp=pp+cp;
		  cp=1;
		  map_p[pp]=2;
		  map_p[pp+cp]=xt;
		  map_p[pp+cp+1]=yt;
		  map_p[pp+cp+2]=xj;
		  map_p[pp+cp+3]=yj;
		  cp+=4;
		  }
		  }
		  else{
		  if(calc_intersection(x0,y0,x,y,xd,yc,xd,yd,&xj,&yj)){
		  xt=xj;yt=yj;
		  if(calc_intersection(x0,y0,x,y,xc,yd,xd,yd,&xj,&yj)){

			map_p[0]++;
			pp=pp+cp;
			cp=1;
			map_p[pp]=2;
			map_p[pp+cp]=xt;
			map_p[pp+cp+1]=yt;
			map_p[pp+cp+2]=xj;
			map_p[pp+cp+3]=yj;
			cp+=4;
			}
			}
			}
			}
			x0=x;y0=y;
			}//else如果不在
			flag=0;
			}
			}
			}
			gpc_input_polygon(0,result,&map_p[0]);
			free(map_p);
			}
*/
int gpc::PointRgn(double x, double y, int Numble, double *PointList)
{
	int i, j;
	int c = 0;
	for (i = 0, j = Numble - 1; i < Numble; j = i++)
	{
		if (((((PointList[i * 2 + 1] <= y) && (y < PointList[j * 2 + 1])) ||
			  ((PointList[j * 2 + 1] <= y) && (y < PointList[i * 2 + 1]))) &&
			 (x < (PointList[j * 2] - PointList[i * 2]) * (y - PointList[i * 2 + 1]) / (PointList[j * 2 + 1] - PointList[i * 2 + 1]) + PointList[i * 2])))
			c = !c;
	}
	return c;
}

BOOL gpc::inPolygon(double x, double y, gpc_polygon *clip)
{
	BOOL b = 0;
	for (int c = 0; c < clip->num_contours; c++)
	{
		b += PointRgn(x, y, clip->contour[c].num_vertices, (double *)clip->contour[c].vertex);
	}
	return b % 2;
}

/*
void gpc::gpc_polyline_clip(gpc_op op, gpc_polygon *subj, gpc_polygon *clip,
gpc_polygon *result)
{
int flag,c,v,cp;
double x,y,xc,yc,xd,yd,xt,yt;
gpc_polygon *p;
double *map_p;
int pp;
double x0,y0,yj,xj;

  pp=1;
  map_p=(double *)malloc(60000*sizeof(double));
  map_p[0]=0;

	p=clip;	//取出裁剪多边形

	  xc=p->contour[0].vertex[0].x;
	  yc=p->contour[0].vertex[0].y;
	  xd=p->contour[0].vertex[2].x;
	  yd=p->contour[0].vertex[2].y;
	  cp=0;
	  p=subj;//取出被裁剪的折线段
	  for (c= 0; c < p->num_contours; c++)
	  {
	  //取出第一点
	  x0=x=p->contour[c].vertex[0].x;
	  y0=y=p->contour[c].vertex[0].y;
	  //判断第一点(x,y)是否在矩形内
	  flag=inPolygon(x,y,clip);
	  if(flag==1){//记录该点，并开始一条折线
	  map_p[0]++;
	  pp=pp+cp;
	  cp=1;
	  map_p[pp]=1;
	  map_p[pp+cp]=x;
	  map_p[pp+cp+1]=y;
	  cp+=2;
	  }
	  //取下一点
	  for (v = 1; v < p->contour[c].num_vertices; v++){
	  x=p->contour[c].vertex[v].x;
	  y=p->contour[c].vertex[v].y;
	  //判断当前点(x,y)是否在矩形内
	  if(inPolygon(x,y,clip)){	//在
	  if(flag){//判断上一点是否在矩形内
	  //在,记录该点，并继续一条折线
	  map_p[pp]++;
	  map_p[pp+cp]=x;
	  map_p[pp+cp+1]=y;
	  x0=x;
	  y0=y;
	  cp+=2;
	  }
	  else{//不在,求交点，并开始一条折线(起始点为交点,有两个点)
	  if(calc_intersection(x0,y0,x,y,xc,yc,xc,yd,&xj,&yj)
	  || calc_intersection(x0,y0,x,y,xc,yc,xd,yc,&xj,&yj)
	  || calc_intersection(x0,y0,x,y,xd,yc,xd,yd,&xj,&yj)
	  || calc_intersection(x0,y0,x,y,xc,yd,xd,yd,&xj,&yj))
	  {
						map_p[0]++;
						pp=pp+cp;
						cp=1;
						map_p[pp]=2;
						map_p[pp+cp]=xj;
						map_p[pp+cp+1]=yj;
						map_p[pp+cp+2]=x;
						map_p[pp+cp+3]=y;
						cp+=4;
						}
						x0=x;y0=y;
						}
						flag=1;
						}
						else{		//不在
						if(flag){//判断上一点是否在矩形内
						//如果在则求交点，并结束本条折线
						if(calc_intersection(x0,y0,x,y,xc,yc,xc,yd,&xj,&yj)
						|| calc_intersection(x0,y0,x,y,xc,yc,xd,yc,&xj,&yj)
						|| calc_intersection(x0,y0,x,y,xd,yc,xd,yd,&xj,&yj)
						|| calc_intersection(x0,y0,x,y,xc,yd,xd,yd,&xj,&yj))
						{
						map_p[pp]++;
						map_p[pp+cp]=xj;
						map_p[pp+cp+1]=yj;
						x0=xj;y0=yj;
						cp+=2;
						}
						else{
						x0=x;y0=y;
						//不可能
						}
						}
						else{//如果不在则判断是否存在两个交点，如果存在则生成一条由两个点组成的折线
						if(calc_intersection(x0,y0,x,y,xc,yc,xc,yd,&xj,&yj)){
						xt=xj;yt=yj;
						if(calc_intersection(x0,y0,x,y,xc,yc,xd,yc,&xj,&yj)
						|| calc_intersection(x0,y0,x,y,xd,yc,xd,yd,&xj,&yj)
						|| calc_intersection(x0,y0,x,y,xc,yd,xd,yd,&xj,&yj)){

						  map_p[0]++;
						  pp=pp+cp;
						  cp=1;
						  map_p[pp]=2;
						  map_p[pp+cp]=xt;
						  map_p[pp+cp+1]=yt;
						  map_p[pp+cp+2]=xj;
						  map_p[pp+cp+3]=yj;
						  cp+=4;
						  }
						  }
						  else{
						  if(calc_intersection(x0,y0,x,y,xc,yc,xd,yc,&xj,&yj)){
						  xt=xj;yt=yj;
						  if(calc_intersection(x0,y0,x,y,xd,yc,xd,yd,&xj,&yj)
						  || calc_intersection(x0,y0,x,y,xc,yd,xd,yd,&xj,&yj)){

							map_p[0]++;
							pp=pp+cp;
							cp=1;
							map_p[pp]=2;
							map_p[pp+cp]=xt;
							map_p[pp+cp+1]=yt;
							map_p[pp+cp+2]=xj;
							map_p[pp+cp+3]=yj;
							cp+=4;
							}
							}
							else{
							if(calc_intersection(x0,y0,x,y,xd,yc,xd,yd,&xj,&yj)){
							xt=xj;yt=yj;
							if(calc_intersection(x0,y0,x,y,xc,yd,xd,yd,&xj,&yj)){

							  map_p[0]++;
							  pp=pp+cp;
							  cp=1;
							  map_p[pp]=2;
							  map_p[pp+cp]=xt;
							  map_p[pp+cp+1]=yt;
							  map_p[pp+cp+2]=xj;
							  map_p[pp+cp+3]=yj;
							  cp+=4;
							  }
							  }
							  }
							  }
							  x0=x;y0=y;
							  }//else如果不在
							  flag=0;
							  }
							  }
							  }
							  gpc_input_polygon(0,result,&map_p[0]);
							  free(map_p);
}*/

/*-------------------------------------------------------------*/
/*         取得一条折线被一个多边形裁剪后的折线                */
/*-------------------------------------------------------------*/
void gpc::gpc_polyline_clip(gpc_op op, gpc_polygon *subj, gpc_polygon *clip,
							gpc_polygon *result)
{
	if ((op != GPC_DIFF) && (op != GPC_INT))
	{
		return;
	}

	double *map_p;		   // 结果， N段，第一段点数，各点坐标，第二段点数，各点坐标
	double *map_m;		   // 中间结果（第一次循环生成），原始折线顶点加交点，结构同上
	double *map_t;		   // 临时结果，每一段与多边形相交得到的结果，点数，坐标，按与第一点的距离进行排序(交点坐标序列)
	double xc, yc, xd, yd; // 裁剪多边形的点（线段）
	double x0, y0, x1, y1; // 折线上的点（线段）
	double xj, yj;		   // 交点坐标

	map_p = (double *)malloc(60000 * sizeof(double));
	map_m = (double *)malloc(60000 * sizeof(double));
	map_t = (double *)malloc(6000 * sizeof(double));
	map_p[0] = 0;
	map_m[0] = 0;

	int pp, i;
	int cp = 1; // 段的点数存在的位置
	// 第一次循环，得到被裁折线原始顶点与交点的序列
	for (i = 0; i < subj->num_contours; i++) // N段
	{
		// 取出第一点并记录
		x0 = subj->contour[i].vertex[0].x;
		y0 = subj->contour[i].vertex[0].y;
		map_m[0]++; // 新的一段

		map_m[cp] = 1; // 此段点数
		pp = 1;		   // 每一段内的指针

		map_m[cp + pp++] = x0;
		map_m[cp + pp++] = y0;

		for (int j = 1; j < subj->contour[i].num_vertices; j++) // 每一线段
		{
			map_t[0] = 0;

			// 折线的线段
			x0 = subj->contour[i].vertex[j - 1].x;
			y0 = subj->contour[i].vertex[j - 1].y;
			x1 = subj->contour[i].vertex[j].x;
			y1 = subj->contour[i].vertex[j].y;

			for (int c = 0; c < clip->num_contours; c++) // 裁剪多边形的多段
			{
				// 多边形的边
				if (clip->contour[c].num_vertices > 1)
				{
					for (int n = 1; n < clip->contour[c].num_vertices; n++) // 每段的每线段
					{
						xc = clip->contour[c].vertex[n - 1].x;
						yc = clip->contour[c].vertex[n - 1].y;
						xd = clip->contour[c].vertex[n].x;
						yd = clip->contour[c].vertex[n].y;
						// 求交点
						if (calc_intersection(x0, y0, x1, y1, xc, yc, xd, yd, &xj, &yj))
						{
							// 每加入一个新点时进行排序
							int k = (int)map_t[0];
							while (k > 0)
							{
								if ((fabs(xj - x0) < fabs(map_t[2 * k - 1] - x0)) || (fabs(yj - y0) < fabs(map_t[2 * k] - y0)))
								{
									map_t[2 * k + 1] = map_t[2 * k - 1];
									map_t[2 * k + 2] = map_t[2 * k];
									k--;
								}
								else
									break;
							}

							// 加入新点
							map_t[2 * k + 1] = xj;
							map_t[2 * k + 2] = yj;
							map_t[0]++;
						}
					}
					//////////////////////////////////////////////////////////////////////////
					xc = clip->contour[c].vertex[0].x;
					yc = clip->contour[c].vertex[0].y;
					xd = clip->contour[c].vertex[clip->contour[c].num_vertices - 1].x;
					yd = clip->contour[c].vertex[clip->contour[c].num_vertices - 1].y;
					// 求交点
					if (calc_intersection(x0, y0, x1, y1, xc, yc, xd, yd, &xj, &yj))
					{
						// 每加入一个新点时进行排序
						int k = (int)map_t[0];
						while (k > 0)
						{
							if ((fabs(xj - x0) < fabs(map_t[2 * k - 1] - x0)) || (fabs(yj - y0) < fabs(map_t[2 * k] - y0)))
							{
								map_t[2 * k + 1] = map_t[2 * k - 1];
								map_t[2 * k + 2] = map_t[2 * k];
								k--;
							}
							else
								break;
						}

						// 加入新点
						map_t[2 * k + 1] = xj;
						map_t[2 * k + 2] = yj;
						map_t[0]++;
					}
					//////////////////////////////////////////////////////////////////////////
				}
			}

			// 加到中间结果中
			map_m[cp] += map_t[0] + 1; // 点数

			for (int m = 0; m < (int)map_t[0]; m++)
			{
				map_m[cp + pp++] = map_t[2 * m + 1];
				map_m[cp + pp++] = map_t[2 * m + 2];
			}

			// 记录被裁折线在此线段的最后一个点
			map_m[cp + pp++] = x1;
			map_m[cp + pp++] = y1;
		}
		cp = cp + pp; // 新一段的点数的位置
	}

	// Debug result
	/*	for (i= 0; i< map_m[1]; i++)
	{
	double sss= map_m[2*i+2];
	double ddd= map_m[2*i+3];
	}
	*/
	// 第二次循环，判断线段是否在多边形内，形成点序列（折线）
	double centX, centY; // 线段中点
	cp = 1;				 // 中间结果的指针
	pp = 1;				 // 段内指针
	int seg = 0;		 // 每段点数指针
	int type;			 // 每一段的类型, 0为环, 1为非环
	int fj, fff = 0;
	int beginP = -1, ggg = 0;

	for (int k = 0; k < (int)map_m[0]; k++) // 多段
	{
		int points = (int)map_m[cp]; // 此段的点数
		ggg = 0;
		fff = 0;

		// 求每一段的中点，判断其是否在多边形内
		for (int m = 1; m < points; m++) // 多点
		{
			x0 = map_m[cp + 2 * m - 1];
			y0 = map_m[cp + 2 * m];
			x1 = map_m[cp + 2 * m + 1];
			y1 = map_m[cp + 2 * m + 2];

			centX = (x0 + x1) / 2;
			centY = (y0 + y1) / 2;

			// 判断操作op，取在多边形内或外的折线段
			int opJudage;
			if (op == GPC_DIFF) // 在多边形外
			{
				opJudage = !inPolygon(centX, centY, clip);
			}
			else // 在多边形内
			{
				opJudage = inPolygon(centX, centY, clip);
			}

			if (fff == 0)
			{
				fj = opJudage;
				fff = 1;
			}
			if (opJudage)
			{
				// 判断前一个点是否已存在
				if ((pp > 1) && (x0 == map_p[pp + seg - 2]) && (y0 == map_p[pp + seg - 1]))
				{
					// 只加后一个点
					map_p[pp + seg] = x1;
					map_p[pp + seg + 1] = y1;
					// 段中点数
					map_p[seg] += 1;
					pp += 2;
				}
				else // 是第一个线段
				{
					// 两个点都加到结果序列中
					seg += pp;
					if (ggg == 0) // 记住此段第一个线段的存储位置
					{
						beginP = seg;
						ggg = 1;
					}

					map_p[seg] = 2; // 点数
					pp = 1;			// 重新开始
					map_p[pp + seg] = x0;
					map_p[pp + seg + 1] = y0;
					map_p[pp + seg + 2] = x1;
					map_p[pp + seg + 3] = y1;
					pp += 4;

					// 段数加1
					map_p[0]++;
				}
			}
		}

		type = subj->hole[k]; // 该段的类型

		if (type == 0 && fj == 1 && (beginP != seg))
		{
			// 特殊处理环状
			double *map_d;											   // 用来置换一下
			map_d = (double *)malloc(2 * (seg + pp) * sizeof(double)); // 最多的情况

			fff = 0;
			ggg = 0;

			int bbb = seg; // 要点数
			int ppp = 0;
			for (i = 0; i < pp; i++) // 最后一段的坐标
			{
				map_d[ppp++] = map_p[bbb++];
			}

			bbb = beginP + 1;
			for (i = 0; i < seg; i++)
			{
				map_d[ppp++] = map_p[bbb++];
			}
			map_d[0] = map_p[beginP] + map_p[seg]; // 修改点数

			// 换回来

			int aaa = beginP;
			bbb = 0;
			for (i = 0; i < ppp; i++)
			{
				map_p[aaa++] = map_d[bbb++];
			}

			map_p[0]--;
			pp -= 2; // 下一个点数位置

			free(map_d);
		}

		cp = cp + 2 * points + 1; // 下一个点数存在的位置
	}

	// Debug result
	/*	int t = 0;
	for (i= 0; i< map_p[0]; i++)
	{
	int up  = map_p[++t];
	for (int j=0; j<up; j++)
	{
	double sss= map_p[++t];
	double ddd= map_p[++t];
	}
	}
	*/
	// 输出结果

	gpc_input_polygon(0, result, &map_p[0]);
	free(map_p);
	free(map_m);
	free(map_t);
}
/*
BOOL gpc::calc_intersection(double xa,double ya,double xb,double yb,
double xc,double yc,double xd,double yd,
double *x, double *y)
{
double tt,t,S;

  tt=(xb-xa)*(yc-yd)-(xc-xd)*(yb-ya);
  t=((xc-xa)*(yc-yd)-(xc-xd)*(yc-ya))/tt;
  S=((xb-xa)*(yc-ya)-(xc-xa)*(yb-ya))/tt;
  if(t>=0 && t<=1 && S>=0 && S<=1){	//存在交点
		tt=(yb-ya)*(xc-xd)-(yd-yc)*(xa-xb);
		*x=((yc*xd-xc*yd)*(xa-xb)-(ya*xb-xa*yb)*(xc-xd))/tt;
		*y=((ya*xb-xa*yb)*(yd-yc)-(yc*xd-xc*yd)*(yb-ya))/tt;
		return TRUE;
		}
		return FALSE;
		}
		*/
// 计算两条线段的交点
BOOL gpc::calc_intersection(double xa, double ya, double xb, double yb,
							double xc, double yc, double xd, double yd,
							double *x, double *y)
{
	double tt, t, S;

	tt = (xb - xa) * (yc - yd) - (xc - xd) * (yb - ya);
	t = ((xc - xa) * (yc - yd) - (xc - xd) * (yc - ya)) / tt;
	S = ((xb - xa) * (yc - ya) - (xc - xa) * (yb - ya)) / tt;
	if (t >= 0 && t <= 1 && S >= 0 && S <= 1)
	{ // 存在交点
		tt = (yb - ya) * (xc - xd) - (yd - yc) * (xa - xb);
		*x = ((yc * xd - xc * yd) * (xa - xb) - (ya * xb - xa * yb) * (xc - xd)) / tt;
		*y = ((ya * xb - xa * yb) * (yd - yc) - (yc * xd - xc * yd) * (yb - ya)) / tt;
		return TRUE;
	}
	return FALSE;
}

BOOL gpc::inBox(double x, double y, double xc, double yc, double xd, double yd)
{
	if (xc > xd)
		swap(&xc, &xd);
	if (yc > yd)
		swap(&yc, &yd);
	if (x >= xc && x <= xd && y >= yc && y <= yd)
		return TRUE;
	else
		return FALSE;
}
