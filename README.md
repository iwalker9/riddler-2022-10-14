# Riddler Classic 2022-10-14
Today I happen to be celebrating the birthday of a family member, which got me wondering about how likely it is for two people in a room to have the same birthday.

Suppose people walk into a room, one at a time. Their birthdays happen to be randomly distributed throughout the 365 days of the year (and no one was born on a leap day). The moment two people in the room have the same birthday, no more people enter the room and everyone inside celebrates by eating cake, regardless of whether that common birthday happens to be today.

On average, what is the expected number of people in the room when they eat cake?

_Extra credit:_ Suppose everyone eats cake the moment three people in the room have the same birthday. On average, what is this expected number of people?

# Solution:
Represent a room where there are N people with different birthdays (or equivalently, there are N birthdays each shared by exactly 1 person) by the 1-dimensional vector (N).

Construct a state tree where with each additional person entering the room, either they share a birthday with somebody in the room with probability N/365 and cake is served, or they do not in which case the state advances to (N+1) with probability (365-N)/365.

The starting state of the room is (0) with probability 1.
* When person 1 enters, the state advances to (1) with probability 1.
* When person 2 enters, cake is served with probability 1/365 and the state advances to (2) with probability 364/365.
* When person 3 enters, cake is served with probability 2/365 and the state advances to (3) with probability 363/365.
* ...and so on until...
* When person 366 enters, we were previously in state (365) so they necessarily share a birthday with somebody and cake is served with probability 1. It is actually fantastically unlikely that we got this far, 1.455e-157, but I ran the calculation with GMP arbitrary-precision rationals to satisfy myself that all of the claimed probabilities really do add up to 1. 

Running the exact EV calculation (1 * P(cake served after 1 person + 2 * P(cake served after 2 people) + ...) gives a fraction with an 864-digit numerator and an 866-digit denominator, close to 24.61659.

# Extra credit solution:
Represent a room where there are N birthdays each shared by exactly 1 person and M birthdays each shared by exactly 2 people by the 2-dimension vector (N, M).

Construct a state tree where with each additional person entering the room, either:
* They share a birthday with two people in the room with probability M/365 and cake is served
* They share a birthday with one person in the room with probability N/365 in which case the state advances to (N-1, M+1)
* They share a birthday with nobody in the room with probability (365-N-M)/365 in which case the state advances to (N+1, M)

Again, the starting state of the room is (0, 0) with probability 1.
* When person 1 enters, the state advances to (1, 0) with probability 1.
* When person 2 enters, the state advances to (0, 1) with probability 1/365 and (2, 0) with probability 364/365. I guess this can be described as a superposition of (0, 1) with weight 1/365 and (2, 0) with weight 364/365, or a matrix with M[0][1] = 1/365, M[2, 0] = 364/365, and all other entries 0.
* From this point, as each new person enters the room, you can examine this probabilistic state and see how each of the possibilities can evolve.  Because you always have P = N + 2M where P is the number of people in the room, the area of interest is limited to a diagonal line running across the state space and the number of distinct states with nonzero weight does not get too large.

Running the EV calculation gives 88.73892.
