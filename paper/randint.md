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
in a determined random number sequence (repeatability), which is useful
for debugging.  However, a global seeding utility is incompatible with the
proposed minimal interface, and causes numerous confusions in a multi-thread
environment.  Instead, an implementation may want to allow user to deploy a
determined seed for debugging purpose.

Different from the initial draft of this paper, only the utility using
`uniform_int_distribution` is proposed; the one using
`uniform_real_distribution` is dropped.  This is because the former forms a
"selection" model, which covers the major use cases, while the later is merely
a simulation of data input.

## Example

    std::randint(0, 6);  // randomly seeded

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
> initialization.  Such a random engine shall be maintained separately for
> each thread.
> *\[Note: The call expressions
> from different threads shall not be able to observe the same pseudo random
> number sequence in a deterministic way.  --end note\]*

    template<class IntType>
      IntType randint(IntType a, IntType b);

> _Requires:_ `a` _&le;_ `b`
> 
> _Effects:_ Produce a random integer _i_, _a &le; i &le; b_, from
> a `uniform_int_distribution<IntType>` (26.5.8.2.1).
> 
> _Returns:_ _i_.

## Sample Implementation

A sample implementation is available at
<https://github.com/lichray/randint> ...You really need this?

## Bikeshed

First of all, overloading `std::rand` is not an option.  User may regard
`std::rand()` as a "safe" variant to the new interface.

Collected so far:

- `randint`, from Python`[2]`
- `random_int`,  by STL
- `pick_int`,  by me, inspired by WEB`[1]`
- `randi`,  by Howard

## Acknowledgments

Hans Boehm, who emphasized the importance of an enforced per-thread random
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
