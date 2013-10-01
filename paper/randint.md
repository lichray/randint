<!-- maruku -o randint.html randint.md -->

<style type="text/css">
pre code { display: block; margin-left: 2em; }
div { display: block; margin-left: 2em; }
ins { text-decoration: none; font-weight: bold; background-color: #A0FFA0 }
del { text-decoration: line-through; background-color: #FFA0A0 }
</style>

<table><tbody>
<tr><th>Doc. no.:</th>	<td>Dnnnn</td></tr>
<tr><th>Date:</th>	<td>2013-09-27</td></tr>
<tr><th>Project:</th>	<td>Programming Language C++, Library Evolution Working Group</td></tr>
<tr><th>Reply-to:</th>	<td>Zhihao Yuan &lt;zy at miator dot net&gt;</td></tr>
</tbody></table>

# std::rand replacement

## Motivation

1. Provide a replacement of `std::rand`, which is considered as an handy
utility (but without a proper interface);
2. Expose the most widely-used combos from C++11 `<random>` without pushing
users to learn the whole design.

## Design Decisions

Seeded before being used.  Seeding, engine, distribution; user gets the three
things altogether, or piece by piece.

Thread-safe.  Minimal interface should minimize astonishment.

All the possible types.  No type promotion; inputs and result have the same
types.

## Examples

    std::randint(0, 6);

## Wording

Change at the end of 26.5.2 rand.synopsis:

    namespace std {
> ...

     // 26.5.7.2, function template generate_canonical
     template<class RealType, size_t bits, class URNG>
     RealType generate_canonical(URNG& g);

<div><ins>
<tt>// 26.5.7.3, global uniform random number generators</tt><br/>
<br/>
<tt>template&lt;class IntType&gt;</tt><br/>
<tt>&nbsp;&nbsp;IntType randint(IntType a, IntType b);</tt><br/>
</ins></div>

     // 26.5.8.2.1, class template uniform_int_distribution
     template<class IntType = int>
     class uniform_int_distribution;
> ...

    } // namespace std

New section 26.5.7.3 rand.util.gurng:

> A global uniform random number generator produces random numbers
> with a non-deterministically seeded `default_random_engine`.  Calls to
> a global URNG shall not introduce a data race (17.6.5.9).
> *\[Note: It is implementation-defined whether multiple global URNGs share
> one random engine to increase randomness, but the same call expressions
> from different threads shall not be able to observe the same pseudo random
> number sequences in a deterministic way.  --end note\]*


> #### 26.5.7.4 Global uniform random number generators

    template<class IntType>
      IntType randint(IntType a, IntType b);

> _Effects:_ Produce a random integer _i_, _a &le; i &le; b_, from
> a `uniform_int_distribution<IntType>` (26.5.8.2.1).
> 
> _Returns:_ _i_.

## Notes

The idea comes from Walter E. Brown's paper, N3742, _Three
`<random>`-related Proposals, v2_.

The interface, `randint`, is taken from the same interface in Python's
`random` package.

It makes sense to me to require those functions `noexcept`, but I also
don't want to forbid an implementation to seed the random engine with
`random_device`.
