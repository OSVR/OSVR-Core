# FlexibleKalmanFilter

## About

This is a C++ template-based header library for building Kalman-style filters, using [Eigen](http://eigen.tuxfamily.org) linear algebra data types and algorithms.

## Variables/Symbols

*n* is the dimension of the state.

*m* is the dimension of (a given) measurement. Note that there can be more than one measurement type involved, that's fine (and desirable!) - only one goes into the `FlexibleKalmanFilter::correct()` function at a time, where most of these are used.

-----

| Variable | Rows | Cols | Meaning                      |
|----------|------|------|------------------------------|
| x        | n    | 1    | **State**                    |
| P        | n    | n    | State error covariance       |
| Q(dt)    | n    | n    | Process model noise covariance|
| z        | m    | 1    | **Measurement**              |
| H        | m    | n    | Measurement Jacobian         |
| R        | m    | m    | Measurement error covariance |
| K        | n    | m    | Kalman gain                  |

## Interfaces and Requirements

There are separate types for State, Process Model, and one or more Measurement types.

The types involved are loosely related/coupled: the Process Model has to know about the particular State type (in some way), but not vice-versa, and the Measurement needs to know (in some way) about State (more about that below), but about no other Measurement types, or the Process Model.

They are all tied together in the `FlexibleKalmanFilter<>` template object, which holds both an instance of Process Model and State, and has a function that takes a (deduced, templated) Measurement argument to its `correct()` method.

Process Model computations of predicted error covariance often take a similar form, so a convenience free function `Matrix<n, n> predictErrorCovariance(State const& state, ProcessModel & model, double dt)` is provided. (If you must know: it performs `A P Atranspose + Q(dt)`)It's optional, but its usage is encouraged if your math matches what it does (as it likely will), so its requirements are also listed below, in addition to the requirements of the `FlexibleKalmanFilter` class methods proper.

Also, a brief note: Most of these types will probably hold what Eigen refers to as a "fixed-size vectorizable" member, so you'll almost certainly want this line in the `public` section of your types:

```c++
EIGEN_MAKE_ALIGNED_OPERATOR_NEW
```

or you'll get weird segfaults if something ends up not being aligned properly.

All the interface stuff below required should be public.

### State

Needs public members:

- `static const types::DimensionType DIMENSION = 12;`
	-	to indicate dimension, aka *n* - here 12. Used all over. (You can instead publicly derive from `HasDimension<12>` or equivalent, to get a nested `integral_constant` type alias named `Dimension` if you prefer types for your constants.)
- `void setStateVector(? state)`
	- to replace the *n*-dimensional state vector, used by `FlexibleKalmanFilter::correct()`
- `Vector<n> stateVector() const`
	- to retrieve the *n*-dimensional state vector: should be `const` and will probably return a `const &` (but if you have a good reason, by value is OK too). Used by `FlexibleKalmanFilter::correct()` and likely many other places.
- `void setErrorCovariance(? errorCovariance)`
	- to replace the *n x n* state error covariance matrix *P*, used by `FlexibleKalmanFilter::correct()`
- `Matrix<n,n> errorCovariance() const`
	- to retrieve the *n x n* state error covariance matrix: should be `const` and will probably return a `const &` (but if you have a good reason, by value is OK too). Used by `FlexibleKalmanFilter::correct()`
- `void postCorrect()` - called at the end of `FlexibleKalmanFilter::correct()` in case your state needs some kind of adjustment at the end. (This is where the "external quaternion" gets updated, for instance.) If you don't need to do anything, just make this do nothing.

### Process Model

Should *not* contain the filter state - that separate object is kept separately and passed as a parameter as needed. It may contain some state (member variables) of its own if required - typically configuration parameters, etc.

- `using State = YourStateType`
	- used for convenience and reducing the number of required parameters to `FlexibleKalmanFilter<>`
- `void predictState(State & state, double dt)`
	- Called by `FlexibleKalmanFilter::predict(dt)`, should update the state to the predicted state after an elapsed `dt` seconds. Often uses the convenience free function `predictErrorCovariance`.
- `Matrix<n, n> getStateTransitionMatrix(State const&, double dt) const`
	- Gets the state transition matrix *A* that represents the process model's effects on the state over the given time interval. You may choose to use this to update the state within `predictState()` (if manual computation is not more efficient). *Optional, but recommended*: **required** if `predictErrorCovariance` is used in `predictState()`
- `Matrix<n, n> getSampledProcessNoiseCovariance(double dt)`
	- Gets the matrix *Q* that represents the local effect of your process on state noise. *Optional, but recommended*: **required** if `predictErrorCovariance` is used in `predictState()`

### Measurement

Note that there may (and often are) several types of measurements used in a particular filter - the `FlexibleKalmanFilter::correct()` method is a function template so it can adjust to any number of measurement types.  If you're having difficulty, check your `const` and try a named instance of your measurement as opposed to passing a temporary.

Only `FlexibleKalmanFilter::correct()` interacts with Measurement types.

- `static const types::DimensionType DIMENSION = 4;`
	-	to indicate dimension of your measurement, aka *m* - here 4. (You can instead publicly derive from `HasDimension<12>` or equivalent, to get a nested `integral_constant` type alias named `Dimension` if you prefer types for your constants.)
- `Vector<m> getResidual(State const& state) const`
	- Also known as the "innovation" or delta *z* - this function predicts the measurement expected given the (predicted) state provided, then takes the difference (contextually defined - may be multiplicative) and returns that, typically by value.
- `Matrix<m,m> getCovariance(State & state) const`
	- Gets the measurement error covariance matrix *R* - a square matrix of dimension equal to measurement dimension. Sometimes a function of state, rather than a fixed value for a measurement, so often returning by value. (Sometimes this doesn't need the state or just needs particular state methods that could be implemented by many state classes, so this could be a function template in a base class, as done with the absolute pose and orientation measurements included.)
- `Matrix<m,n> getJacobian(State & s) const`
	- Gets the measurement Jacobian *H*: the change in what you'd expect the measurement to be, with respect to changes in the state, given the (predicted) state. Thus, it has rows equal to the measurement dimension, and cols equal to the state dimension. (If you're using a SCAAT-style filter, much of this will be zero.) This usually required closest coupling to the State type out of all the methods, so if you're splitting things up, this could be in the derived class, as done with the absolute pose and orientation measurements included. This is nearly always a function that ends up returning by value.

## Acknowledgments

In-code references are often to "Welch 1996" - this is to Greg Welch's PhD dissertation. Specifically, the citation follows, and the link is publicly accessible for the full text.

- Welch, G. F. (1996). SCAAT: Incremental Tracking with Incomplete Information. University of North Carolina at Chapel Hill. Retrieved from <http://www.cs.unc.edu/techreports/96-051.pdf>

Portions of that dissertation were published as "Welch and Bishop 1997" - if you can get a copy of that paper, it is more condensed, but as far as I can tell everything we use is inside the dissertation. (The link is the canonical DOI link, you'll need some library subscription to get the paper from that place.)

- Welch, G. F., & Bishop, G. (1997). SCAAT: incremental tracking with incomplete information. In Proceedings of the 24th annual conference on Computer graphics and interactive techniques - SIGGRAPH ’97 (pp. 333–344). New York, NY, USA: ACM Press/Addison-Wesley Publishing Co. <http://doi.org/10.1145/258734.258876>

Welch gives two citations for the "externalized-quaternion, internal linear incremental orientation" concept, only one of which I could get a copy of, and which is cited in the code. (As above, the link is the canonical one and behind a paywall.)

- Azarbayejani, A., & Pentland, A. P. (1995). Recursive estimation of motion, structure, and focal length. Pattern Analysis and Machine Intelligence, IEEE Transactions on, 17(6), 562–575. <http://doi.org/10.1109/34.387503>

### Library structure

Library structure/design was inspired by

- "Eigen-Kalman <https://github.com/rpavlik/eigen-kalman> and/in turn by [TAG](http://www.edwardrosten.com/cvd/tag.html) whose source is now accessible at <https://github.com/edrosten/tag/tree/master/tag>.