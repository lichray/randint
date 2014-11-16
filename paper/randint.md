<!-- maruku -o randint.html randint.md -->

<style type="text/css">
pre code { display: block; margin-left: 2em; }
div { display: block; margin-left: 2em; }
ins { text-decoration: none; font-weight: bold; background-color: #A0FFA0 }
del { text-decoration: line-through; background-color: #FFA0A0 }
</style>

<table><tbody>
<tr><th>Doc. no.:</th>	<td>Dnnnn</td></tr>
<tr><th>Date:</th>	<td>2014-11-08</td></tr>
<tr><th>Project:</th>	<td>Programming Language C++, SG6: Numerics</td></tr>
<tr><th>Reply-to:</th>	<td>Zhihao Yuan &lt;zy at miator dot net&gt;</td></tr>
</tbody></table>

# std::rand replacement, revision 2

## Changes since N4217

- Rename the seeding utility to `reseed`, which resets the distributions'
  states if needed, based on SG6 comments.
- Cleanup the wording with the term "unpredictable state" stolen from Walter's
  paper`[1]`.
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

    std::reseed();                   // with default_seed
    std::shuffle(begin(v), end(v));

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
<br/>
<div><ins>
<tt>// 26.5.7.4, seeding the per-thread engine</tt><br/>
<tt>void reseed();</tt><br/>
<tt>void reseed(default_random_engine::result_type value);</tt><br/>
</ins></div>

     // 26.5.8.2.1, class template uniform_int_distribution
     template<class IntType = int>
       class uniform_int_distribution;
> ...

    } // namespace std

New section 26.5.7.3 &#91;rand.util.randint&#93;:

> #### 26.5.7.3 function template `randint`

> All specializations of the function template described in this section
> share the same `default_random_engine` for a given execution of
> a thread; the random engine is
> set to an unpredictable state during the initialization.
> Such a random engine shall be maintained separately for
> each thread.

    template<class IntType>
      IntType randint(IntType a, IntType b);

> _Requires:_ `a` _&le;_ `b`
>
> _Effects:_ Produce a random integer _i_, _a &le; i &le; b_, from
> a `uniform_int_distribution<IntType>` (26.5.8.2.1).
>
> _Returns:_ _i_.

New section 26.5.7.4 &#91;rand.util.reseed&#93;:

> #### 26.5.7.4 seeding the per-thread engine

    void reseed();
    void reseed(default_random_engine::result_type value);

> Let `g` be the random engine defined in section 26.5.7.3 in the same
> thread.
>
> _Effects:_ The first form invokes `g.seed()`.
> The second form invokes `g.seed(value)`.
>
> _Postcondition_: Subsequent uses of any specializations of `randint`
> (26.5.7.3) do
> not depend on values produced by `g` prior to this call.

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
> _Remarks:_ To the extent that the implementation of this function makes use
> of random numbers, the object `g` shall serve as the implementation's source
> of randomness<ins> in the second form, so does the random engine
> defined in section 26.5.7.3 in the same thread to the first form</ins>.

The following wording is tentatively relative to N4082 **&#91;fund.ts&#93;**.

Change 10.3 &#91;alg.random.sample&#93;:

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
> - To the extent that the implementation of this function makes use
> of random numbers, the object `g` shall serve as the implementation's source
> of randomness<ins> in the second form, so does the random engine
> defined in section 26.5.7.3 in the same thread to the first form</ins>.

## Sample Implementation

A sample implementation is available at
<https://github.com/lichray/randint>.

## Bikeshedding

First of all, overloading `std::rand` is not an option.  User may deem
`std::rand()` as a "safe" variant to the new interface.

Collected so far:

- `randint`, from Python`[2]`
- `rand_int`, by Nicolai and Andy Sawyer
- `random_int`,  by STL
- `pick_int`,  by me, inspired by WEB`[1]`
- `randi`,  by Howard

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
