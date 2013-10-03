<!-- maruku -o randint.html randint.md -->

<style type="text/css">
pre code { display: block; margin-left: 2em; }
div { display: block; margin-left: 2em; }
ins { text-decoration: none; font-weight: bold; background-color: #A0FFA0 }
del { text-decoration: line-through; background-color: #FFA0A0 }
</style>

<table><tbody>
<tr><th>Doc. no.:</th>	<td>Nnnnn</td></tr>
<tr><th>Date:</th>	<td>2013-10-02</td></tr>
<tr><th>Project:</th>	<td>Programming Language C++, Library Evolution Working Group</td></tr>
<tr><th>Reply-to:</th>	<td>Zhihao Yuan &lt;zy at miator dot net&gt;</td></tr>
</tbody></table>

# std::rand replacement

## Motivation

We want to deprecate `std::rand` friends, while "deprecation without a
replacement" is a valid concern.  This paper

1. Propose replacement to the `std::rand` friends.  As a global uniform
random number generator, `std::rand` is considered both handy and useful.

2. Expose the most widely-used combos from C++11 `<random>` without pushing
the users to learn the whole design of `<random>`.  Smoothing the learning
curve can usually optimize the acceptance.

## Design Decisions

`std::rand` is a single interface, and its "replacement" should be able to be
used as a single interface as well.  In addition, I expect the interface to
correctly expose the functionalities of `<random>` and lead to more robust
and secure programs.  The proposed replacement is

- Distribution based.  RNG must be used with a distribution; `std::rand`
is just a wrong design.

- Randomly seeded before being used.  Improper seeding results in
vulnerability, like `rand(time(0))`.

- Thread-safe.  Minimal interface should minimize astonishment.

- Templated.  No type promotion; inputs and result have the same types.

Seeding a pseudo random number generator with a determined value will result
in a determined sequence of random numbers (repeatability), which is useful
for debugging.  So a re-seeding utility is also proposed.  However, there are
some differences between this interface and the `seed` member functions:

- Seeding without an argument performs random seeding.  Because default
`seed()` sets a random engine to the default state, while the default state
of the global random engine is randomly seeded.

- Seed sequence is not supported.  The effect of using seed sequence from
multiple threads is unpredicatable.

Different from the initial draft of this paper, only the utility using
`uniform_int_distribution` is proposed; the one using
`uniform_real_distribution` is dropped.  This is because the former forms a
"selection" model, which covers the major use cases, while the later is merely
a simulation of data input.

## Examples

    std::randint(0, 6);  // randomly seeded
    std::reseed(0);      // help debugging in a single thread program
    std::reseed();       // random again

## Wording

Change 26.5.2 rand.synopsis:

    namespace std {
> ...

     // 26.5.7.2, function template generate_canonical
     template<class RealType, size_t bits, class URNG>
     RealType generate_canonical(URNG& g);

<div><ins>
<tt>// 26.5.7.3, function template randint</tt><br/>
<tt>template&lt;class IntType&gt;</tt><br/>
<tt>&nbsp;&nbsp;IntType randint(IntType a, IntType b);</tt><br/>
<br/>
<tt>// 26.5.7.4, function reseed</tt><br/>
<tt>void reseed();</tt><br/>
<tt>void reseed(default_random_engine::result_type s);</tt><br/>
</ins></div>

     // 26.5.8.2.1, class template uniform_int_distribution
     template<class IntType = int>
     class uniform_int_distribution;
> ...

    } // namespace std

New section 26.5.7.3 rand.util.randint:

> #### 26.5.7.3 function template `randint`

> All functions instantiated from the template described in this section
> 26.5.7.3 share the same `default_random_engine` for a given execution of
> a thread; the random engine is non-deterministically seeded during the
> initialization.  Calls to any instantiation shall not introduce a data
> race (17.6.5.9).
> *\[Note: It is implementation-defined whether the instantiations in
> different threads share
> the same random engine to increase randomness, but the call expressions
> from different threads shall not be able to observe the same pseudo random
> number sequences in a deterministic way.  --end note\]*

    template<class IntType>
      IntType randint(IntType a, IntType b);

> _Requires:_ `a` _&le;_ `b`
> 
> _Effects:_ Produce a random integer _i_, _a &le; i &le; b_, from
> a `uniform_int_distribution<IntType>` (26.5.8.2.1).
> 
> _Returns:_ _i_.

New section 26.5.7.4 rand.util.reseed:

> #### 26.5.7.4 function `reseed`

> The overloaded functions described in this section 26.5.7.4 seed the
> random engine described in 26.5.7.3.
> *\[Note:  Calls to these functions may introduce data race.  --end note\]*

*\[Editorial-note:  And the repeatability may only be observable when
`randint` is being used in a single thread.  --end note\]*

    void reseed();

> _Effects:_ With `g` as the random engine, invokes `g.seed(s)`, where
> `s` is a non-deterministic value of type
> `default_random_engine::result_type`.

    void reseed(default_random_engine::result_type s);

> _Effects:_ With `g` as the random engine, invokes `g.seed(s)`.

## Future Issues

It makes sense to me to require those functions `noexcept`, but I also
don't want to forbid an implementation to seed the random engine with
`random_device`.

## References

`[1]` Brown, Walter E. N3742 _Three `<random>`-related Proposals, v2_.
      <http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2013/n3742.pdf>

`[2]` _`random` -- Generate pseudo-random numbers_.
      "The Python Standard Library".
      <http://docs.python.org/2/library/random.html>
