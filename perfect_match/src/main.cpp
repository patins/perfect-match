#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <float.h>
using namespace std;

const unsigned N = 1000;
const unsigned D = 10;

float* generate_user_info()
{
  srand(42);
  float *user_info = (float*) malloc(N*D*sizeof(float));
  for (unsigned i = 0; i < N; i++) {
    for (unsigned j = 0; j < D; j++)
      user_info[N*i + j] = rand();
  }
  return user_info;
}

float user_delta(float* user_info, unsigned i, unsigned j) {
  float delta = 0;
  for (unsigned k = 0; k < D; k++)
    delta += fabs(user_info[N*i + k] - user_info[N*j + k]);
  return delta;
}

unsigned match_count(bool* user_matches, unsigned i) {
  unsigned count = 0;
  for (unsigned j = 0; j < N; j++)
    count += user_matches[N*i + j];
  return count;
}

float compute_objective(float* user_info, bool* user_matches) {
  float objective = 0;
  for (unsigned i = 0; i < N; i++) {
    for (unsigned j = i + 1; j < N; j++) {
      if (user_matches[N*i + j])
        objective += user_delta(user_info, i, j);
    }
  }
  return objective;
}

bool* match_users_greedy(float* user_info, unsigned b) {
  bool* user_matches = (bool*) calloc(N*N, sizeof(bool));

  while (true) {
    float best_delta = FLT_MAX;
    unsigned best_i, best_j;

    for (unsigned i = 0; i < N; i++) {
      if (match_count(user_matches, i) >= b)
        continue;

      for (unsigned j = i+1; j < N; j++) {
        if (match_count(user_matches, i) >= b || match_count(user_matches, j) >= b)
          continue;

        float delta = user_delta(user_info, i, j);
        if (!user_matches[N*i + j] && delta < best_delta) {
          best_delta = delta;
          best_i = i;
          best_j = j;
        }
      }
    }

    if (best_delta < FLT_MAX) {
      user_matches[N*best_i + best_j] = true;
      user_matches[N*best_j + best_i] = true;
      cout << "matched " << best_i << " and " << best_j << " obj = " << compute_objective(user_info, user_matches) << endl;
    } else {
      cout << "failed to unmatch" << endl;
      break;
    }
  }

  return user_matches;
}


int main()
{
  float* user_info = generate_user_info();
  bool* user_matches = match_users_greedy(user_info, 1);
  cout << "obj " << compute_objective(user_info, user_matches) << endl;
  /*for (unsigned i = 0; i < N; i++) {
    for (unsigned j = 0; j < D; j++)
      cout << user_info[N*i + j] << " ";

    cout << endl;
  }*/
  /*
  for (unsigned i = 0; i < N; i++) {
    for (unsigned j = 0; j < N; j++)
      cout << user_delta(user_info, i, j) << " ";
    cout << endl;
    break;
  }
  */

  return 0;
}
