#pragma once 

#include "cstdlib"


namespace MiniSTL {

// Return a random number in the range [0, n).  This function encapsulates
// whether we're using rand (part of the standard C library) or lrand48
// (not standard, but a much better choice whenever it's available).

template <class Distance>
inline Distance __random_number(Distance n) {
    return rand() % n;
}

// random_shuffle
template <class RandomIt, class RandomNumberGenerator>
void random_shuffle(RandomIt first, RandomIt last,
                    RandomNumberGenerator& rand = __random_number) {
	if (first == last)
		return;
	for(RandomIt i = first + 1; i != last; ++i)
		iter_swap(i, first + rand((i - first) + 1));
}

// random_sample and random_sample_n
template <class ForwardIt, class OutputIt, class Distance,
          class RandomNumberGenerator>
OutputIt random_sample_n(ForwardIt first, ForwardIt last,
                         OutputIt out, const Distance n,
                         RandomNumberGenerator& rand = __random_number) {
	Distance remaining = distance(first, last);
	Distance m(min(n, remaining));

	while(m > 0) {
		if (rand(remaining) < m) {
			*out = *first;
			++out;
			--m;
		}

		--remaining;
		++first;
	}
	return out;
}

template <class InputIt, class RandomIt,
          class RandomNumberGenerator, class Distance>
RandomIt random_sample(InputIt first, InputIt last,
                       RandomIt out, const Distance n,
					   RandomNumberGenerator& rand = __random_number) {
	Distance m(0);
	Distance t(n);
	for(;first != last && m < n; ++m, ++first)
		out[m] = *first;

	while(first != last) {
		++t;
		Distance M = rand(t);
		if(M < n)
			out[M] = *first;
		++first;
	}

	return out + m;
}

template <class InputIt, class RandomIt, 
          class RandomNumberGenerator>
inline RandomIt
random_sample(InputIt first, InputIt last,
              RandomIt out_first, RandomIt out_last,
              RandomNumberGenerator& rand = __random_number) {	
  	return random_sample(first, last,
                         out_first, 
                         out_last - out_first,
						 rand);
}

} // MiniSTL

