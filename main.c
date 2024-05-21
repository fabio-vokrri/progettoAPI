#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define add_station_msg "aggiungi-stazione"
#define remove_station_msg "demolisci-stazione"
#define add_car_msg "aggiungi-auto"
#define remove_car_msg "rottama-auto"
#define plan_route_msg "pianifica-percorso"

#define added_msg "aggiunta"
#define not_added_msg "non aggiunta"
#define removed_msg "demolita"
#define not_removed_msg "non demolita"
#define scrapped_msg "rottamata"
#define not_scrapped_msg "non rottamata"
#define no_route_msg "nessun percorso"

#define delim " \n"

#define buffer_size 8192
#define max_cars 512

// the input buffer
char *buffer;

// for convenience
typedef enum boolean {
  false,
  true
} boolean;

/**
 * @brief color of the RB tree nodes
 */
typedef enum color {
  red,
  black,
} color_t;

/**
 * @brief node of the RB tree
 */
typedef struct station {
  int distance;
  color_t color;
  struct station *left;
  struct station *right;
  struct station *parent;
  int number_of_cars;
  int max_range;
  int *cars;
} station_t;

// null station -> leaf in RB tree
station_t *null_station = NULL;
// root of RB tree
station_t *root = NULL;

/*** FUNCTION DECLARATION ***/
char *get_token();

int ascending(const void *, const void *);

int descending(const void *, const void *);

void swap(int *, int, int);

int contains_car(station_t *, int);

station_t *init_station(int);

station_t *get_at(int);

station_t *get_minimum(station_t *);

station_t *get_maximum(station_t *);

station_t *get_successor_of(station_t *);

station_t *get_predecessor_of(station_t *);

void left_rotate(station_t *);

void right_rotate(station_t *);

void insert_fixup(station_t *);

void delete_fixup(station_t *);

void transplant(station_t *, station_t *);

station_t *add_station(int);

boolean remove_station(int);

boolean add_car(station_t *, int);

boolean remove_car(station_t *, int);

int *optimize(station_t **, int);

int *explore_forward(station_t *, station_t *);

int *explore_backward(station_t *, station_t *);

int *plan_route(int, int);


void play();

/*** FUNCTION DEFINITION ***/

/**
 * @brief gets the next token from the input buffer
 */
char *get_token() {
  // gets the number of chars before it finds the delimiter
  int len = strcspn(buffer, delim); // NOLINT(*-narrowing-conversions)
  // if no other chars available
  if (len == 0)
    return NULL;

  // replaces the space with a terminator character,
  buffer[len] = '\0';
  // and gets the command
  char *curr = buffer;
  // moves the pointer to the start of the next command
  buffer += len + 1;

  return curr;
}

/**
 * @brief compares the two given elements. USED FOR ASCENDING ORDER.
 *
 * @param a the first element.
 * @param b the second element.
 * @return int indicates the order of the elements.
 */
int ascending(const void *a, const void *b) {
  return ((station_t *) a)->distance - ((station_t *) b)->distance;
}

/**
 * @brief compares the two given elements. USED FOR DESCENDING ORDER.
 *
 * @param a the first element.
 * @param b the second element.
 * @return int indicates the order of the elements.
 */
int descending(const void *a, const void *b) {
  return *(int *) b - *(int *) a;
}

/**
 * @brief swaps the elements at the given indexes of the given array.
 *
 * @param arr the array to swap the elements in.
 * @param i the first index.
 * @param j the second index.
 */
void swap(int *arr, int i, int j) {
  int temp = arr[i];
  arr[i] = arr[j];
  arr[j] = temp;
}

/**
 * @brief searches the car with the given range in the given station.
 *
 * @param station the station where to search for the car.
 * @param range the range of the car to search.
 * @return int the index at which the car == found, -1 if no car == found.
 */
int contains_car(station_t *station, int range) {
  if (station == NULL || station == null_station || station->number_of_cars == 0)
    return -1;

  // sorts the array in descending order
  qsort(station->cars, station->number_of_cars, sizeof(int), descending);

  // searches for the given range in the array.
  // binary search algorithm
  int l = 0, r = station->number_of_cars - 1;
  while (l <= r) {
    int mid = (int) (l + r) / 2;
    if (station->cars[mid] == range)
      return mid;

    if (range > station->cars[mid]) {
      r = mid - 1;
    } else
      l = mid + 1;
  }

  // no car found
  return -1;
}

/**
 * @brief allocates memory and initializes a new station at the given distance.
 *
 * @param distance the distance where to put the station.
 * @return station_t* the newly created station.
 */
station_t *init_station(int distance) {
  station_t *new_station = malloc(sizeof(station_t));

  if (new_station != NULL) {
    // initializes the distance field with the given distance,
    // and all the other fields to default values
    new_station->distance = distance;
    new_station->color = black;
    new_station->left = null_station;
    new_station->right = null_station;
    new_station->parent = null_station;
    new_station->cars = (int *) calloc(max_cars, sizeof(int)); // array of 512 integers, all initialized to 0
    new_station->max_range = 0;
    new_station->number_of_cars = 0;

    return new_station;
  }

  return NULL;
}

/**
 * @brief gets the station at the given distance.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param distance the distance at where the station should be found.
 * @return station_t* the found station, or NULL if none == found.
 */
station_t *get_at(int distance) {
  station_t *x = root;
  while (x != NULL && x != null_station && x->distance != distance) {
    if (distance < x->distance)
      x = x->left;
    else
      x = x->right;
  }

  return x;
}

/**
 * @brief gets the station with the minimum distance from the given station.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param x the station from where to begin the search.
 * @return station_t* the station with the minimum distance.
 */
station_t *get_minimum(station_t *x) {
  if (x == NULL || x == null_station)
    return null_station;

  while (x->left != null_station)
    x = x->left;

  return x;
}

/**
 * @brief gets the station with the maximum distance from the given station.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param x the station from where to begin the search.
 * @return station_t* the station with the maximum distance.
 */
station_t *get_maximum(station_t *x) {
  if (x == NULL || x == null_station)
    return null_station;

  while (x->right != null_station)
    x = x->right;

  return x;
}

/**
 * @brief gets the station next to the given one in the route.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param station the station from where to begin the search.
 * @return station_t* the next station.
 */
station_t *get_successor_of(station_t *x) {
  if (x == null_station)
    return null_station;

  if (x->right != null_station)
    return get_minimum(x->right);

  station_t *y = x->parent;
  while (y != null_station && x == y->right) {
    x = y;
    y = y->parent;
  }
  return y;
}

/**
 * @brief gets the station previous to the given one in the route.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param station he station from where to begin the search.
 * @return station_t* the previous station.
 */
station_t *get_predecessor_of(station_t *x) {
  if (x == null_station)
    return null_station;

  if (x->left != null_station)
    return get_maximum(x->left);

  station_t *y = x->parent;
  while (y != null_station && x == y->left) {
    x = y;
    y = y->parent;
  }

  return y;
}

/**
 * @brief rotates the sub tree with the given root to the left.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param x the root of the sub tree to rotate.
 */
void left_rotate(station_t *x) {
  if (x->right == null_station)
    return;

  station_t *y = x->right;
  x->right = y->left;

  if (y->left != null_station)
    y->left->parent = x;
  y->parent = x->parent;

  if (x->parent == null_station)
    root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;

  y->left = x;
  x->parent = y;
}

/**
 * @brief rotates the sub tree with the given root to the right.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param x the root of the sub tree to rotate.
 */
void right_rotate(station_t *x) {
  if (x->left == null_station)
    return;

  station_t *y = x->left;
  x->left = y->right;

  if (y->right != null_station)
    y->right->parent = x;
  y->parent = x->parent;

  if (x->parent == null_station)
    root = y;
  else if (x == x->parent->right)
    x->parent->right = y;
  else
    x->parent->left = y;

  y->right = x;
  x->parent = y;
}

/**
 * @brief fixes the tree properties after an insertion.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param z the station that was inserted.
 */
void insert_fixup(station_t *z) {
  station_t *y = NULL;

  while (z->parent->color == red) {
    if (z->parent == z->parent->parent->left) {
      y = z->parent->parent->right;
      if (y->color == red) {
        z->parent->color = black;
        y->color = black;
        z->parent->parent->color = red;
        z = z->parent->parent;
      } else {
        if (z == z->parent->right) {
          z = z->parent;
          left_rotate(z);
        }
        z->parent->color = black;
        z->parent->parent->color = red;
        right_rotate(z->parent->parent);
      }
    } else {
      y = z->parent->parent->left;
      if (y->color == red) {
        z->parent->color = black;
        y->color = black;
        z->parent->parent->color = red;
        z = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          z = z->parent;
          right_rotate(z);
        }
        z->parent->color = black;
        z->parent->parent->color = red;
        left_rotate(z->parent->parent);
      }
    }
  }

  root->color = black;
}

/**
 * @brief fixes the tree properties after a deletion.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param z the station that was deleted.
 */
void delete_fixup(station_t *z) {
  station_t *w = NULL;

  if (z == NULL)
    return;

  while (z != root && z->color == black) {
    if (z == z->parent->left) {
      w = z->parent->right;

      if (w->color == red) {
        w->color = black;
        z->parent->color = red;
        left_rotate(z->parent);
        w = z->parent->right;
      }

      if (w->left->color == black && w->right->color == black) {
        w->color = red;
        z = z->parent;
      } else {
        if (w->right->color == black) {
          w->left->color = black;
          w->color = red;
          right_rotate(w);
          w = z->parent->right;
        }

        w->color = z->parent->color;
        z->parent->color = black;
        w->right->color = black;
        left_rotate(z->parent);
        z = root;
      }
    } else {
      w = z->parent->left;

      if (w->color == red) {
        w->color = black;
        z->parent->color = red;
        right_rotate(z->parent);
        w = z->parent->left;
      }

      if (w->right->color == black && w->left->color == black) {
        w->color = red;
        z = z->parent;
      } else {
        if (w->left->color == black) {
          w->right->color = black;
          w->color = red;
          left_rotate(w);
          w = z->parent->left;
        }

        w->color = z->parent->color;
        z->parent->color = black;
        w->left->color = black;
        right_rotate(z->parent);
        z = root;
      }
    }
  }

  z->color = black;
}

/**
 * @brief transplants the sub tree with root v in the sub tree with root u.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param u the root of the sub tree where to transplant.
 * @param v the root of the sub tree to transplant.
 */
void transplant(station_t *u, station_t *v) {
  if (u->parent == null_station)
    root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  v->parent = u->parent;
}

/**
 * @brief adds the station at the given distance in the given route.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param distance the distance to add the station.
 * @return station_t* the added station, or NULL if already present.
 */
station_t *add_station(int distance) {
  station_t *y = null_station;
  station_t *x = root;

  while (x != NULL && x != null_station) {
    // the station already exists
    if (x->distance == distance)
      return NULL;

    y = x;
    if (distance < x->distance)
      x = x->left;
    else
      x = x->right;
  }

  station_t *z = init_station(distance);
  if (z != NULL) {
    z->parent = y;

    if (y == null_station)
      root = z;
    else if (distance < y->distance)
      y->left = z;
    else
      y->right = z;

    z->color = red;

    insert_fixup(z);
    return z;
  }

  return null_station;
}

/**
 * @brief removes the station at the given distance in the given route.
 * * DO NOT TOUCH! From Cormen book
 *
 * @param distance the distance to remove the station at.
 * @return true if the station has been added successfully.
 * @return false otherwise.
 */
boolean remove_station(int distance) {
  station_t *z = get_at(distance);
  if (z == null_station)
    return false;

  station_t *y = z;
  station_t *x = NULL;
  color_t color = y->color;

  if (root == z && z->left == null_station && z->right == null_station) {
    root = NULL;
  } else if (z->left == null_station) {
    x = z->right;
    transplant(z, z->right);
  } else if (z->right == null_station) {
    x = z->left;
    transplant(z, z->left);
  } else {
    y = get_minimum(z->right);
    color = y->color;
    x = y->right;

    if (y->parent == z)
      x->parent = y;
    else {
      transplant(y, y->right);
      y->right = z->right;
      y->right->parent = y;
    }

    transplant(z, y);
    y->left = z->left;
    y->left->parent = y;
    y->color = z->color;
  }

  if (color == black)
    delete_fixup(x);

  free(z->cars);
  free(z);
  return true;
}

/**
 * @brief adds the car in the given station with the given range.
 *
 * @param station the station where to add the car.
 * @param range the range of the car to be added.
 * @return true if the car has been added successfully.
 * @return false otherwise.
 */
boolean add_car(station_t *station, int range) {
  if (station == NULL || station == null_station || station->number_of_cars == max_cars)
    return false;

  // checks if the given range is grater than the maximum one in the given station
  if (range > station->max_range)
    station->max_range = range;

  // adds the car in last available position
  station->cars[station->number_of_cars++] = range;

  return true;
}

/**
 * @brief removes the car with the given range from the given station.
 *
 * @param station the station from which to remove the car.
 * @param range the range of the car to be removed.
 * @return true if the car has been removed successfully.
 * @return false otherwise.
 */
boolean remove_car(station_t *station, int range) {
  if (station == NULL || station == null_station || station->number_of_cars == 0)
    return false;

  // checks if the car exists in the array of cars and retrieves its index.
  // this function sorts the array in descending order
  int i = contains_car(station, range);
  if (i != -1) {
    // sets its range to 0 and swaps with the last available car
    station->cars[i] = 0;
    swap(station->cars, i, --station->number_of_cars);

    // fixes the maximum range available in the given station
    if (i == 0)
      station->max_range = station->cars[1];
    else
      station->max_range = station->cars[0];

    return true;
  }

  return false;
}

/**
 * @brief optimizes the given route to be minimal.
 *
 * @param unoptimized an unoptimized minimum path
 * @param length the length of the route
 * @return int* the minimum optimized path
 */
int *optimize(station_t **unoptimized, int length) {
  int i = length - 1;
  station_t *current = unoptimized[i],
      *target = unoptimized[i];

  while (true) {
    current = get_successor_of(current);

    // reached the end of the array
    if (current == unoptimized[0])
      break;

    if (i >= 1 && current == unoptimized[i - 1]) {
      target = current;
      i--;
      continue;
    } else {
      // checks if the current station can reach the target one and
      // if it can be reached from the next station in the array.
      if (
          i >= 2 && current->distance - current->max_range <= target->distance &&
          unoptimized[i - 2]->distance - unoptimized[i - 2]->max_range <= current->distance &&
          current->distance < unoptimized[i - 1]->distance //
          ) {
        // inserts the node in the output array, replacing the unoptimized one.
        unoptimized[i - 1] = current;
        current = unoptimized[i - 1];
        target = current;
        i--;

      } else
        continue;
    }
  }

  // inserts all the distances in the output array (including -1 as a delimiter)
  int *output = (int *) malloc(sizeof(int) * (length + 1));
  for (int j = 0; j <= length; j++) {
    output[j] = unoptimized[j]->distance;
  }

  free(unoptimized);
  return output;
}

/**
 * @brief explores the minimum path from station1 to station2 in the given route.
 * this is called when the distance of station1 is smaller than the station2 distance.
 * it takes advantage of the fact that the chosen stations must have the smallest distance from the origin.
 *
 * @param station1 the first station.
 * @param station2 the second station.
 * @return int* the minimum path from station1 to station2.
 */
int *explore_forward(station_t *station1, station_t *station2) {
  station_t *current = station2,
      *best = station2,
      *target = station2;

  int *output = (int *) malloc(sizeof(int));
  output[0] = station2->distance;
  int length = 1;

  while (true) {
    // finds the best node that can reach the target station
    while (current != station1) {
      current = get_predecessor_of(current);
      if (current->distance + current->max_range >= target->distance)
        best = current;
    }

    // if the best node is the target one, no path exists
    if (best == target) {
      free(output);
      return NULL;
    }

    // otherwise insert the best station in the output array
    output = (int *) realloc(output, sizeof(int) * (length + 1));
    output[length++] = best->distance;

    // if the best node is the arrival station, it found the best path
    if (best == station1) {
      // sorts in ascending order
      qsort(output, length, sizeof(int), ascending);
      // inserts in the output array -1 as a delimiter
      output = (int *) realloc(output, sizeof(int) * (length + 1));
      output[length] = -1;

      return output;
    }

    target = best;
    current = best;
  }
}

/**
 * @brief explores the minimum path from station1 to station2 in the given route.
 * this is called when the distance of station1 is grater than the distance of station2.
 *
 * @param station1 the first station.
 * @param station2 the second station.
 * @return int* the minimum path from station1 to station2.
 */
int *explore_backward(station_t *station1, station_t *station2) {
  station_t *current = station1,
      *target = station1,
      *best = station1;

  station_t **unoptimized = malloc(sizeof(station_t));
  unoptimized[0] = station1;

  int length = 1;

  while (true) {
    if (current == station2) {
      // if the arrival station cannot be reached from the target station no path exists
      if (current->distance + target->max_range < target->distance) {
        free(unoptimized);
        return NULL;
      }

      // otherwise, insert the last station and the null station as a delimiter
      unoptimized = realloc(unoptimized, sizeof(station_t) * (length + 2));
      unoptimized[length++] = current;
      unoptimized[length] = null_station;

      // the found path has the minimum number of nodes,
      // but is not the best one, so needs to be optimized
      return optimize(unoptimized, length);
    }

    current = get_predecessor_of(current);
    // checks if the current station can be reached from the target one
    // and if it has the best (minimum) score
    if (
        current->distance + target->max_range >= target->distance &&
        current->distance - current->max_range <= best->distance - best->max_range //
        )
      best = current;
    else {
      // checks if the current is the predecessor of the target
      // or if the best station has already been inserted into the output array
      if (current == get_predecessor_of(target) || best == unoptimized[length - 1]) {
        free(unoptimized);
        return NULL;
      }

      // otherwise insert the best node into the output array
      unoptimized = realloc(unoptimized, sizeof(station_t) * (length + 1));
      unoptimized[length++] = best;
      target = best;
      current = target;
    }
  }
}

/**
 * @brief plans the route from the given distance to the given distance.
 *
 * @param distance1 the distance from which to plan the route.
 * @param distance2 the distance of the station to be reached.
 * @return int* an array of distances indicating the stations where to stop and change car.
 */
int *plan_route(int distance1, int distance2) {
  // the route has no station or only one: it's impossible to plan a route.
  if (root == NULL || (root->right == null_station && root->left == null_station))
    return NULL;

  int *output = NULL;

  // checks if the stations exists in the route.
  station_t *station1 = get_at(distance1),
      *station2 = get_at(distance2);
  if (station1 == null_station || station2 == null_station)
    return NULL;

  // if the given distances are the same
  if (distance1 == distance2) {
    output = malloc(sizeof(int) * 2);
    output[0] = distance1;
    output[1] = -1;

    return output;
  }

  // two different algorithms for forward and backward paths
  if (distance1 < distance2)
    output = explore_forward(station1, station2);
  else
    output = explore_backward(station1, station2);

  return output;
}

/**
 * @brief reads the input from stdin and calls the appropriate methods.
 */
void play() {
  // flushes the standard input.
  fflush(stdin);

  // the input tends to repeat multiple operations to the same station,
  // so cache the last added station to avoid searching for it again.
  station_t *cached = null_station;

  // pointer to the first element of the buffer
  // in order to reset it before fetching another command.
  char *temp = buffer;

  // game main loop
  while (true) {
    // resets the buffer.
    buffer = temp;

    // if no more commands retrieved from standard input,
    // the program must terminate.
    if (fgets(buffer, buffer_size, stdin) == NULL)
      break;

    // gets the current command from input
    char *command = get_token();

    // adds station
    if (strcmp(command, add_station_msg) == 0) {
      // adds the new station at the provided distance.
      int distance = atoi(get_token());
      station_t *station = add_station(distance);

      // the station is already present in the route.
      if (station == NULL) {
        puts(not_added_msg);
        continue;
      }

      // caches the station.
      cached = station;
      puts(added_msg);

      // adds the provided number of cars in the station.
      int number_of_cars = atoi(get_token());
      for (int i = 0; i < number_of_cars; i++) {
        int range = atoi(get_token());
        add_car(station, range);
      }

      continue;
    }

    // adds car
    if (strcmp(command, add_car_msg) == 0) {
      station_t *station;
      int distance = atoi(get_token());

      // retrieves the cached station instead of searching it
      // if the provided distance is the same.
      if (distance == cached->distance)
        station = cached;
      else {
        // if it's not the same, searches the new station and caches it.
        station = get_at(distance);
        if (station == null_station) {
          puts(not_added_msg);
          continue;
        }

        cached = station;
      }

      // adds the car with the specified range in the station.
      int range = atoi(get_token());
      if (add_car(station, range))
        puts(added_msg);
      else
        puts(not_added_msg);

      continue;
    }

    // removes car
    if (strcmp(command, remove_car_msg) == 0) {
      station_t *station;
      int distance = atoi(get_token());

      // retrieves the cached station instead of searching it
      // if the provided distance is the same.
      if (distance == cached->distance)
        station = cached;
      else {
        // if it's not the same, searches the new station and caches it.
        station = get_at(distance);
        if (station == null_station) {
          puts(not_scrapped_msg);
          continue;
        }

        cached = station;
      }

      // removes the car with the specified range from the station.
      int range = atoi(get_token());
      if (remove_car(station, range))
        puts(scrapped_msg);
      else
        puts(not_scrapped_msg);

      continue;
    }

    // removes station
    if (strcmp(command, remove_station_msg) == 0) {
      int distance = atoi(get_token());

      // checks if the remove_station function does indeed remove the station
      // and if the cached one is the same as the one removed.
      if (remove_station(distance)) {
        puts(removed_msg);
        if (distance == cached->distance)
          cached = null_station;
      } else
        puts(not_removed_msg);

      continue;
    }

    // plans route
    if (strcmp(command, plan_route_msg) == 0) {
      int distance1 = atoi(get_token());
      int distance2 = atoi(get_token());
      int *planned_route = plan_route(distance1, distance2);

      // if no path exists
      if (planned_route == NULL) {
        puts(no_route_msg);
        continue;
      }

      // prints the planned route
      int i = 1;
      while (planned_route[i] != -1) {
        printf("%d ", planned_route[i - 1]);
        i++;
      }
      printf("%d\n", planned_route[i - 1]);

      free(planned_route);
      continue;
    }
  }
}

/**
 * @brief program execution entry point.
 *
 * @return int 0 if the program successfully executed.
 */
int main() {
  // initializes the input buffer
  buffer = (char *) malloc(buffer_size);
  // initializes the leaf node of the RB tree
  null_station = init_station(-1);
  // initializes the game
  play();

  return 0;
}