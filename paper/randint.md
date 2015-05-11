<!-- maruku -o randint.html randint.md -->

<style type="text/css">
pre code { display: block; margin-left: 2em; }
div { display: block; margin-left: 2em; }
ins { text-decoration: none; font-weight: bold; background-color: #A0FFA0 }
del { text-decoration: line-through; background-color: #FFA0A0 }
</style>

<table><tbody>
<tr><th>Doc. no.:</th>	<td>Dnnnn</td></tr>
<tr><th>Date:</th>	<td>2015-05-08</td></tr>
<tr><th>Project:</th>	<td>Programming Language C++, Library
Working Group</td></tr>
<tr><th>Revises:</th>	<td>N4316</td></tr>
<tr><th>Reply-to:</th>	<td>Zhihao Yuan &lt;zy at miator dot net&gt;</td></tr>
</tbody></table>

# std::rand replacement, revision 3

* TOC
{:toc}

## Changes since N4316

- `reseed()` (without argument) randomizes the engine.
- Uninterested sections removed.
- Feature-testing recommendation added.
- Wording reworked.

## Changes since N4217

- Rename the seeding utility to `reseed`, which resets the distributions'
  states if needed, based on SG6 comments.
- Cleanup the wording with the term "unpredictable state" stolen from Walter's
  paper`[1]`.
- Ask the question about exposing the per-thread engine in Future Issues.
- Update motivation to reflect the status quo.

## Changes since N3796

- Seeding utility `seed_init` based on SG6's preference.
- Variants for `std::shuffle` and `std::experimental::sample`.
- Naming candidate `rand_int`.
- Some outdated discussions are removed.

## Motivation

The `std::rand` friends are discouraged in C++14, so we want:

1. A direct replacement to the `std::rand` friends.  Despite of the security
issues, `std::rand` is considered both handy and useful as a global uniform
random number generator.

2. To expose the most widely-used combo in C++11 `<random>` without
pushing the users to learn the whole design.  Smoothing the learning
curve can usually optimize the acceptance.

## Design Decisions

`std::rand` is a self-contained interface, so its replacement should be
independent as well.  In addition, I expect the interface to
correctly expose the functionalities of `<random>` and lead to more robust
and secure programs.  The proposed replacement is

- Distribution based.  RNG must be used with a distribution; `std::rand`
is a wrong design.

- Randomly seeded before used.  Improper seeding like `rand(time(0))`
may result in vulnerabilities.

- Per-thread engine.  Minimal interface should minimize astonishment, wrt.
thread-safety and performance.

- Manually seedable.  User can observe repeatability in a given thread, which
is a typical demand for debugging.

- Type-safe.  No integral promotion, no loss of distribution property during
down-casting.  For a given invocation, the inputs and
the result have the same type.

Two variants for the shuffling and sampling algorithms without the explicit
`URNG` parameter are also proposed.

## Example

    std::randint(0, 6);              // randomly seeded
    std::randint(0L, 6L);            // deduced type
    std::randint<size_t>(0, 6);      // selected type

    std::reseed(0);                  // for debugging purpose
    std::shuffle(begin(v), end(v));
    std::reseed();                   // back to random

## Wording

Change 26.5.2 &#91;rand.synopsis&#93;:

    namespace std {
> ...

     // 26.5.7.2, function template generate_canonical
     template<class RealType, size_t bits, class URNG>
       RealType generate_canonical(URNG& g);

<div><ins>
<tt>// 26.5.7.3, function template randint</tt><br/>
<tt>template&lt;class IntType&gt;</tt><br/>
<tt>&nbsp;&nbsp;IntType randint(IntType a, IntType b);</tt><br/>
</ins></div>
<div><ins>
<tt>void reseed();</tt><br/>
<tt>void reseed(default_random_engine::result_type value);</tt><br/>
</ins></div>

     // 26.5.8.2.1, class template uniform_int_distribution
     template<class IntType = int>
       class uniform_int_distribution;
> ...

    } // namespace std

New section 26.5.7.3 &#91;rand.util.randint&#93;:

> #### 26.5.7.3 Function template `randint`

> A separate _per-thread engine_ of type `default_random_engine`,
> initialized to an unpredictable state,
> shall be maintained for each thread.

    template<class IntType>
      IntType randint(IntType a, IntType b);

> _Requires:_ `a` _&le;_ `b`.  If the template argument does not meet the
> requirements for `IntType` (26.5.1.1),
> the program is ill-formed.
>
> _Returns:_ A random integer _i_, _a &le; i &le; b_, produced from
> a thread-local instance of `uniform_int_distribution<IntType>` (26.5.8.2.1)
> invoked with the per-thread engine.

    void reseed();
    void reseed(default_random_engine::result_type value);

> _Effects:_ Let `g` be the per-thread engine (26.5.7.3).
> The first form sets `g` to an unpredictable state.
> The second form invokes `g.seed(value)`.
>
> _Postcondition:_ Subsequent calls to `randint`
> (26.5.7.3) do
> not depend on values produced by `g` before calling `reseed`.
> *\[Note:*
> `reseed` also resets any instances of `uniform_int_distribution` used
> by `randint`.
> *--end note\]*

Change 25.1 &#91;algorithms.general&#93;:

> ### Header `<algorithm>` synopsis
> ...

     // 25.3.12, shuffle:

<div><ins>
<tt>template&lt;class RandomAccessIterator&gt;</tt><br/>
<tt>&nbsp;&nbsp;void shuffle(RandomAccessIterator first,
RandomAccessIterator last);</tt>
</ins></div>

     template<class RandomAccessIterator, class UniformRandomNumberGenerator>
       void shuffle(RandomAccessIterator first,
                    RandomAccessIterator last,
                    UniformRandomNumberGenerator&& g);

Change 25.3.12 &#91;alg.random.shuffle&#93;:

<div><ins>
<tt>template&lt;class RandomAccessIterator&gt;</tt><br/>
<tt>&nbsp;&nbsp;void shuffle(RandomAccessIterator first,
RandomAccessIterator last);</tt>
</ins></div>

     template<class RandomAccessIterator, class UniformRandomNumberGenerator>
       void shuffle(RandomAccessIterator first,
                    RandomAccessIterator last,
                    UniformRandomNumberGenerator&& g);

> ...
>
> _Remarks:_
> <ins>If `g` is not given in the argument list, it denotes the per-thread
> engine (26.5.7.3).</ins>
> To the extent that the implementation of this function makes use
> of random numbers, the object `g` shall serve as the implementation's source
> of randomness.

The following wording is relative to N4529 **&#91;fund.ts2&#93;**.

> *\[Editorial note:*
> Please consider referring the C++17 working paper.
> *--end note\]*

Change 12.3 &#91;alg.random.sample&#93;:

<div><ins>
<tt>template&lt;class PopulationIterator, class SampleIterator,
class Distance&gt;</tt><br/>
<tt>SampleIterator sample(PopulationIterator first, PopulationIterator
last,</tt><br/>
<tt>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;SampleIterator
out, Distance n);</tt>
</ins></div>

     template<class PopulationIterator, class SampleIterator,
              class Distance, class UniformRandomNumberGenerator>
     SampleIterator sample(PopulationIterator first, PopulationIterator last,
                           SampleIterator out, Distance n,
                           UniformRandomNumberGenerator&& g);

> ...
>
> _Remarks:_
>
> - Stable if and only if `PopulationIterator` meets the requirements of a
> `ForwardIterator` type.
> - <ins>If `g` is not given in the argument list, it denotes the per-thread
> engine (26.5.7.3).</ins> To the extent that the implementation of this function makes use of random numbers, the object `g` shall serve as the implementation's source of randomness.

## Feature-testing recommendation

Please use `randint` as the macro name suffix in the library category.

## Sample Implementation

A sample implementation is available at
<https://github.com/lichray/randint>.

## Acknowledgments

Hans Boehm, who emphasized the importance of enforcing the per-thread random
engine more than once.

Stephan T. Lavavej, who carefully reviewed this paper and provided many
corrections.

Walter E. Brown, who drafted the paper`[1]` which contains basically the same
thought.

And many others who joined the `std::rand` related discussions on _c++std-lib_
and _c++std-lib-ext_ mailing lists.

## References

`[1]` Brown, Walter E. N3742 _Three `<random>`-related Proposals, v2_.
      <http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3742.pdf>

`[2]` _`random` -- Generate pseudo-random numbers_.
      "The Python Standard Library".
      <http://docs.python.org/2/library/random.html>
