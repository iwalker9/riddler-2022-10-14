#include <gmp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXTARGET 3

// Representation of an "inner state", i.e. not a winning state.
typedef struct {
  // The number of different birthdays shared by 0, 1, ... people
  uint16_t counts[MAXTARGET];
  // The probability that the room will be in this state at the current iteration
  mpq_t weight;
} innerstate_t;

void advance_state(uint8_t target, const innerstate_t *in, innerstate_t **out, mpq_t cake) {
  for(uint8_t i = 0; i < target; i++) {
    if(in->counts[i] > 0) {
      // Take a copy of the prior state
      innerstate_t new = *in;

      // If we're not calculating the final transition to the cake state, then
      // the new state will have one fewer birthday shared by i people and one
      // more shared by i+1 people (namely, this is the birthday of the new
      // person who just entered the room).
      if(i < target - 1) {
        new.counts[i]--;
        new.counts[i + 1]++;
      }

      // The probability of making this transition is the number of birthdays
      // shared by i people divided by 365.
      mpq_t p;
      mpq_init(p);
      mpq_set_ui(p, in->counts[i], 365);
      mpq_init(new.weight);
      mpq_mul(new.weight, in->weight, p); // free

      // If this is a non-cake transition, append it to the output list.
      // Otherwise, add this probability to the probability that we have reached
      // cake.
      if(i < target - 1) {
#ifdef DEBUG
        printf("[%d, %d] <", in->counts[0], in->counts[1]);
        mpq_out_str(stdout, 10, in->weight); // read
        printf("> -> [%d, %d] @ %lp with p=%d/365 <", new.counts[0], new.counts[1], *out, in->counts[i]);
        mpq_out_str(stdout, 10, new.weight);
        printf(">\n");
#endif
        *((*out)++) = new;
      } else {
#ifdef DEBUG
        printf("[%d, %d] <", in->counts[0], in->counts[1]);
        mpq_out_str(stdout, 10, in->weight);
        printf("> wins with p=%d/365 <", in->counts[i]);
        mpq_out_str(stdout, 10, new.weight);
        printf(">\n");
#endif
        mpq_add(cake, cake, new.weight);
        mpq_clear(new.weight);
      }

      mpq_clear(p);
    }
  }
}

int compar_states(const void *a, const void *b) {
  const innerstate_t *sa = a;
  const innerstate_t *sb = b;
  for(int i = 0; i < MAXTARGET; i++) {
    if(sa->counts[i] != sb->counts[i]) {
      return sa->counts[i] - sb->counts[i];
    }
  }
  return 0;
}

int main(int argc, char *argv[]) {
  uint8_t target = 3;
  
  // Starting state has 365 birthdays shared by 0 people (because the room is
  // empty) with probability 1.
  innerstate_t start;
  start.counts[0] = 365;
  for(uint8_t i = 1; i < MAXTARGET; i++) {
    start.counts[i] = 0;
  }
  
  mpq_init(start.weight);
  mpq_set_ui(start.weight, 1, 1);

  size_t len = 1;
  innerstate_t *states = malloc(len * sizeof(innerstate_t));
  states[0] = start;

  mpq_t cake;
  mpq_init(cake);

  mpq_t cake_last;
  mpq_init(cake_last);

  for(int i = 0; i < 400; i++) {
    innerstate_t *newstates = malloc(len * (target - 1) * sizeof(innerstate_t));
    innerstate_t *out = newstates;
    for(size_t j = 0; j < len; j++) {
      advance_state(target, &states[j], &out, cake);

      // This node is done, so free its weight
      mpq_clear(states[j].weight);
    }
    len = out - newstates;
    free(states);
    states = newstates;

    // Sort the list of states and remove duplicates.  This avoids exponential
    // growth in the list.  Possibly an explicit sparse matrix representation
    // would be preferable for the N=2 case
    qsort(states, len, sizeof(innerstate_t), compar_states);
    {
#ifdef DEBUG
      printf("before compacting:\n");
      for(size_t j = 0; j < len; j++) {
        const innerstate_t *in = states + j;
        printf("[%d", in->counts[0]);
        for(int k = 1; k < target; k++) {
          printf(", %d", in->counts[k]);
        }
        printf("]\n");
      }
#endif
      
      innerstate_t *out = states;
      for(size_t j = 1; j < len; j++) {
        innerstate_t *in = states + j;

        if(compar_states(in, out) == 0) {
          mpq_add(out->weight, out->weight, in->weight);
          mpq_clear(in->weight);
        } else {
          out++;
          if(out != in) {
            memcpy(out, in, sizeof(innerstate_t));
          }
        }
      }
      len = out - states + 1;
      
      
#ifdef DEBUG
      printf("after compacting:\n");
      for(size_t j = 0; j < len; j++) {
        const innerstate_t *in = states + j;
        printf("[%d", in->counts[0]);
        for(int k = 1; k < target; k++) {
          printf(", %d", in->counts[k]);
        }
        printf("]\n");
      }        
#endif
    }

    double p_cake = mpq_get_d(cake);
    mpq_t delta;
    mpq_init(delta);
    mpq_sub(delta, cake, cake_last);
    double p_cake_delta = mpq_get_d(delta);
    mpq_clear(delta);
    printf("With %d people in the room, P(cake) = %.8lf, P(just hit cake)=%.8lf\n", i + 1, p_cake, p_cake_delta);
    mpq_set(cake_last, cake);
  }

  for(size_t i = 0; i < len; i++) {
    mpq_clear(states[i].weight);
  }
  free(states);
  mpq_clear(cake);
  mpq_clear(cake_last);
  
  return 0;
}
