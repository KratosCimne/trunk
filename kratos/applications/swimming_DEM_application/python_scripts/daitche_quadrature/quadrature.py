import math 
import cmath
import mpmath
import matplotlib.pyplot as plt
from bigfloat import *
import sys

# *****************************************************************************************************************************************************************************************
# EXACT EVALUATIONS
# *****************************************************************************************************************************************************************************************

def ExactIntegrationOfSinus(t, a = None, b = None):
    with precision(300):
        if a == None and b == None:
            return 0.5 * math.pi * math.sqrt(t) * (mpmath.angerj(0.5, t) - mpmath.angerj(- 0.5, t))
        elif a == None and b != None:
            a = 0
        elif b == None:
            b = t
        mpmath.mp.dps = 50
        mpmath.mp.pretty = True
        pi = mpmath.mp.pi
        pi = +pi
        fcos = mpmath.fresnelc
        fsin = mpmath.fresnels        
        arg_a = mpmath.sqrt(2 * (t - a) / pi)
        arg_b = mpmath.sqrt(2 * (t - b) / pi) 
        return mpmath.sqrt(2 * mpmath.mp.pi) * ((fsin(arg_b) - fsin(arg_a)) * mpmath.cos(t) + (fcos(arg_a) - fcos(arg_b)) * mpmath.sin(t))
      
def ExactIntegrationOfSinusWithExponentialKernel(t, ti, alpha = None, beta = None):
    a = sqrt(exp(1) / ti)
    b = - 0.5  / ti
    return a / (b ** 2 + 1) * (exp(b * (t - alpha)) * (b * sin(alpha) + cos(alpha)) - exp(b * (t - beta)) * (b * sin(beta) + cos(beta)))

# *****************************************************************************************************************************************************************************************
# QUADRATURES
# *****************************************************************************************************************************************************************************************

# ApproximateQuadrature BEGINS
# -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
def ApproximateQuadrature(times, f):
    values = [0.0 for t in times]
    acc_sum = 2 * math.sqrt(times[-1] - times[-2]) * f(times[-1])
    
    for i in range(len(values) - 1):
        if i == 0:
            delta_t = times[1] - times[0]
        else:
            delta_t = times[i] - times[i - 1]
                    
        acc_sum += 0.5 * delta_t * (f(times[i]) + f(times[i - 1])) / math.sqrt(times[-1] - times[i])
        
    return acc_sum
# -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# ApproximateQuadrature ENDS

# Daitche BEGINS
# -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

def Alpha(n, j):
    four_thirds = 4. / 3
    exponent = 1.5
        
    if 0 < j and j < n:
        return four_thirds * ((j - 1) ** exponent + (j + 1) ** exponent - 2 * j ** exponent)
    elif j == 0:
        return four_thirds
    else:
        return four_thirds * ((n - 1) ** exponent - n ** exponent + exponent * sqrt(n))

def Beta(n, j):
    sqrt_2 = math.sqrt(2)
    sqrt_3 = math.sqrt(3)
    sqrt_n = math.sqrt(n) 
    
    if n >= 4:
        if 2 < j and j < n - 1:
            return 8. / 15 * (  (j + 2) ** 2.5 - 3 * (j + 1) ** 2.5 + 3 * j ** 2.5 - (j - 1) ** 2.5)\
                   + 2. / 3 * (- (j + 2) ** 1.5 + 3 * (j + 1) ** 1.5 - 3 * j ** 1.5 + (j - 1) ** 1.5)
        elif j == 0:
            return 4. / 5 * sqrt_2
        elif j == 1:
            return  14. / 5 * sqrt_3 - 12. / 5 * sqrt_2
        elif j == 2:
            return 176. / 15 - 42. / 5 * sqrt_3 + 12. / 5 * sqrt_2
        elif j == n - 1:
            return 8. / 15 * (- 2 * n ** 2.5 + 3 * (n - 1) ** 2.5 - (n - 2) ** 2.5)\
                  + 2. / 3 * (  4 * n ** 1.5 - 3 * (n - 1) ** 1.5 + (n - 2) ** 1.5)
        else:
            return 8. / 15 * (n ** 2.5 - (n - 1) ** 2.5) + 2. / 3 * (- 3 * n ** 1.5 + (n - 1) ** 1.5) + 2 * sqrt_n
        
    elif n == 2:
        if j == 0:
            return 12. / 15 * sqrt_2
        elif j == 1:
            return 16. / 15 * sqrt_2
        else:
            return 2. / 15 * sqrt_2
        
    else:
        if j == 0:
            return 4. / 5 * sqrt_2
        elif j == 1:
            return 14. / 5 * sqrt_3 - 12. / 5 * sqrt_2
        elif j == 2:
            return - 8. / 5 * sqrt_3 + 12. / 5 * sqrt_2
        else:
            return 4. / 5 * sqrt_3 - 4. / 5 * sqrt_2

def Gamma(n, j):

    with precision(200):
        sqrt_2 = sqrt(2)
        sqrt_3 = sqrt(3)
        sqrt_5 = sqrt(5)
        sqrt_6 = sqrt(6) 
        sqrt_n = sqrt(n) 
        one = BigFloat(1)
        
        if n >= 7:  
            if 3 < j and j < n - 3:
                return 16. / (one * 105) * (      (j + 2) ** (one * 3.5)     + (j - 2) ** (one * 3.5) - 4 * (j + 1) ** (one * 3.5) - 4 * (j - 1) ** (one * 3.5) + 6 * j ** (one * 3.5))\
                            + 2. / (one * 9) * (4 * (j + 1) ** (one * 1.5) + 4 * (j - 1) ** (one * 1.5)     - (j + 2) ** (one * 1.5)     - (j - 2) ** (one * 1.5) - 6 * j ** (one * 1.5))
            elif j == 0:
                return 244. / (one * 315) * sqrt_2
            elif j == 1:
                return 362. / (one * 105) * sqrt_3 - 976. / (one * 315) * sqrt_2
            elif j == 2:
                return 5584. / (one * 315) - 1448. / (one * 105) * sqrt_3 + 488. / (one * 105) * sqrt_2
            elif j == 3:
                return 1130. / (one * 63) * sqrt_5 - 22336. / (one * 315) + 724. / (one * 35) * sqrt_3 - 976. / (one * 315) * sqrt_2   
            elif j == n - 3:
                return 16. / (one * 105) * (n ** (one * 3.5) - 4 * (n - 2) ** (one * 3.5)     + 6 * (n - 3) ** (one * 3.5)      - 4 * (n - 4) ** (one * 3.5)          + (n - 5) ** (one * 3.5))\
                        - 8. / (one * 15) * n ** (one * 2.5) + 4. / (one * 9) * n ** (one * 1.5) + 8. / (one * 9) * (n - 2) ** (one * 1.5) - 4. / (one * 3) * (n - 3) ** (one * 1.5) + 8. / (one * 9) * (n - 4) ** (one * 1.5) - 2. / (one * 9) * (n - 5) ** (one * 1.5) 
            elif j == n - 2:
                return 16. / (one * 105) * ((n - 4) ** (one * 3.5) - 4 * (n - 3) ** (one * 3.5)      + 6 * (n - 2) ** (one * 3.5)            - 3 * n ** (one * 3.5))\
                            + 32. / (one * 15) * n ** (one * 2.5)       - 2 * n ** (one * 1.5) - 4. / (one * 3) * (n - 2) ** (one * 1.5) + 8. / (one * 9) * (n - 3) ** (one * 1.5) - 2. / (one * 9) * (n - 4) ** (one * 1.5)
            elif j == n - 1:
                return 16. / (one * 105) * (3 * n ** (one * 3.5) - 4 * (n - 2) ** (one * 3.5) + (n - 3) ** (one * 3.5)) - 8. / (one * 3) * n ** (one * 2.5) + 4 * n ** (one * 1.5) + 8. / (one * 9) * (n - 2) ** (one * 1.5) - 2. / (one * 9) * (n - 3) ** (one * 1.5)
            else:
                return 16. / (one * 105) * ((n - 2) ** (one * 3.5) - n ** (one * 3.5)) + 16. / (one * 15) * n ** (one * 2.5) - 22. / (one * 9) * n ** (one * 1.5) - 2. / (one * 9) * (n - 2) ** (one * 1.5) + 2 * sqrt_n
        elif n == 3:
            if j == 0:
                return 68. / 105 * sqrt_3
            elif j == 1:
                return 6. / 7 * sqrt_3
            elif j == 2:
                return 12. / 35 * sqrt_3  
            else:
                return 16. / 105 * sqrt_3
        elif n == 4:
            if j == 0:
                return 244. / 315 * sqrt_2
            elif j == 1:
                return 1888. / 315 - 976. / 315 * sqrt_2
            elif j == 2:
                return - 656. / 105 + 488. / 105 * sqrt_2
            elif j == 3:
                return 544. / 105 - 976. / 315 * sqrt_2
            else:
                return - 292. / 315 + 244. / 315 * sqrt_2
        elif n == 5:
            if j == 0:
                return 244. / 315 * sqrt_2
            elif j == 1:
                return 362. / 105 * sqrt_3 - 976. / 315 * sqrt_2
            elif j == 2:
                return 500. / 63 * sqrt_5 - 1448. / 105 * sqrt_3 + 488. / 105 * sqrt_2
            elif j == 3:
                return - 290. / 21 * sqrt_5 + 724. / 35 * sqrt_3 - 976. / 315 * sqrt_2
            elif j == 4:
                return 220. / 21 * sqrt_5 - 1448. / 105 * sqrt_3 + 244. / 315 * sqrt_2            
            else:
                return - 164. / 63 * sqrt_5 + 362. / 105 * sqrt_3
        else:
            if j == 0:
                return 244. / 315 * sqrt_2
            elif j == 1:
                return 362. / 105 * sqrt_3 - 976. / 315 * sqrt_2
            elif j == 2:
                return 5584. / 315 - 1448. / 105 * sqrt_3 + 488. / 105 * sqrt_2
            elif j == 3:
                return 344. / 21 * sqrt_6 - 22336. / 315 + 724. / 35 * sqrt_3 - 976. / 315 * sqrt_2
            elif j == 4:
                return - 1188. / 35 * sqrt_6 + 11168. / 105 - 1448. / 105 * sqrt_3 + 244. / 315 * sqrt_2
            elif j == 5:
                return 936. / 35 * sqrt_6 - 22336. / 315 + 362. / 105 * sqrt_3
            else:
                return - 754. / 105 * sqrt_6 + 5584. / 315
        
def Coefficient(order, n, j):
    if order == 1:
        return Alpha(n, j)
    elif order == 2:
        return Beta(n, j)
    else:         
        return Gamma(n, j)

def Daitche(times, f, order):
    t = times[- 1]
    t0 = times[0]
    sqrt_of_h = math.sqrt(times[-1] - times[-2])    
    n = len(times) - 1
    total = 0.0   
    
    for j in range(0 , n + 1):
        coefficient = Coefficient(order, n, j)
        total += coefficient * f(times[-j - 1])
    
    return sqrt_of_h * total
# -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Daitche ENDS

# Hinsberg BEGINS
# -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

def Phi(t):
    if abs(t) > 1e-7:
        answer = (exp(t) - 1.) / t
    else:
        answer = 1. + 0.5 * t + 1. / 6 * t ** 2
    return answer

def Hinsberg(m, t_win, times, f):
    verify_interpolation = False
    cheat = False
    cheat_a_little = True
    print_debug_info = False
    
    if len(times) < 4: # no tail left
        return Daitche(times, f, 1)
    else:
        import hinsberg_optimization as op
        initial_time = times[0]
        final_time = times[- 1]
        interval = final_time - initial_time
        
        # Partitioning time vector in two  ------------------------------
               
        for i in range(len(times)):
            if times[i] >= final_time - t_win:
                break
            
        t_middle = times[i]    
        old_times = [time * t_middle / interval for time in times]        
        recent_times = [t_middle + time * (interval - t_middle) / interval for time in times]
        
        # Calculating window integral  ------------------------------
        
        #F_win = Bombardelli(recent_times, f, 4)#(1, recent_times, f)
        F_win = Daitche(recent_times, f, 3)
        
        # Calculating Tail Integral ------------------------------
        
        # Building exponentials interpolation of the kernel
        
        tis = [0.1, 0.3, 1., 3., 10., 40., 190., 1000., 6500., 50000.]        
        a0 = [0.2 for ti in tis]
        functional = op.Functional(tis)
        op.GetExponentialsCoefficients(functional, a0)
        print("times of tangency: ", tis)
        print("a coefficeints: ", a0)
        F_tail = float(ExactIntegrationOfSinus(final_time, 0.0, old_times[1]))
        Fis = [0.0 for coefficient in a0]
        
        # Verifying interpolation by exponentials
        
        if verify_interpolation:
            plot_times = [i * 0.01 for i in range(500)]
            approx_values = []
            exact_values = []
            for time in plot_times:
                approximation_value = 0.0
                for i in range(len(a0)):
                    ti = tis[i]
                    a = sqrt(exp(1) / ti)
                    b = - 0.5  / ti
                    approximation_value += a0[i] * a * exp(b * time)
                approx_values.append(approximation_value)
                exact_values.append(1 / sqrt(time))
            
            plt.plot(plot_times, approx_values)
            plt.plot(plot_times, exact_values)
            plt.axis('equal')
            plt.show()
        
        # For each interpoland, calculate its integral contribution step by step
        
        for i in range(len(a0)):
            ti = tis[i]
            for k in range(1, len(old_times)):
                delta_t = old_times[k] - old_times[k - 1]                
                normalized_dt = delta_t / (2 * ti)
                normalized_t = old_times[k - 1] / (2 * ti)
                fn = f(old_times[k - 1])
                fn_plus_1 = f(old_times[k])
                exp_dt = exp(- normalized_dt)
                Fdi = 2 * sqrt(exp(1.) * ti) * exp(- normalized_t) * (fn * (1 - Phi(- normalized_dt)) + fn_plus_1 * exp_dt * (Phi(normalized_dt) - 1.)) #
                if cheat_a_little: # calculate exact contribution and not approximation given by Hinsberg
                    Fdi = ExactIntegrationOfSinusWithExponentialKernel(final_time - old_times[- 1] + k * delta_t, ti, old_times[k - 1], old_times[k])
                Fre = exp_dt * Fis[i]
                Fis[i] = float(Fdi) + Fre
                
            F_tail += a0[i] * Fis[i]
            
        # Cheating, calculating the exact interpoland's integral all at once (and not step by step) ------------------------------
        
        if cheat:
            F_tail = 0.0
            for i in range(len(a0)):
                ti = tis[i]
                F_tail += a0[i] * ExactIntegrationOfSinusWithExponentialKernel(final_time, ti, 0., old_times[-1])
        
        
        # Printing debug info ------------------------------
        
        if print_debug_info:
            print("times", times)
            print("old_times", old_times)
            print("recent_times", recent_times)   
            print("EXACT tail", ExactIntegrationOfSinus(final_time, 0., t_win))        
            print("EXACT recent", ExactIntegrationOfSinus(final_time, t_win, final_time))         
            print("EXACT whole", ExactIntegrationOfSinus(final_time))        
            print("WHOLE", Daitche(times, f, 3))  
            print("RECENT", F_win)                
            print("TAIL", F_tail)
            print("F_win + F_tail", F_win + F_tail)
            sys.exit
        return F_win + F_tail

# -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Hinsberg ENDS

# Bombardelli BEGINS
# -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

def OmegaTaylorTerms(alpha, bi, pi):
    first = bi
    second = bi * (pi - alpha / 2)
    third = bi * (3 * alpha ** 2 + alpha + 12 * pi ** 2 - 12 * alpha * pi) / 24
    fourth = bi * (alpha * (15 * alpha ** 3 + 30 * alpha ** 2 + 5 * alpha - 2) + 240 * pi ** 4 - 480 * alpha * pi ** 3 + 120 * alpha * (3 * alpha + 1) * pi ** 2 - 120 * alpha ** 2 * (alpha + 1) * pi) / 5760
    return first, second, third, fourth

def A(alpha, p, f, t, times, c = 1): # A^alpha_{ch,p}f(cx), Baeumer (2015)
    t = times[- 1]
    a = times[0]
    N = len(times) - 1
    h = (t - a) / N
    return h ** (- alpha) * sum([gamma(k - alpha) / gamma(k + 1) * f(t - (k - p) * h) for k in range(N)])

def Bombardelli(times, f_to_integrate, order = 1):
    
    with precision(200):
        f_to_integrate
        q = - 0.5
        t = times[- 1]
        a = times[0]
        N = len(times) - 1
        h = (t - a) / N
        initial_approx_deriv = 0.5 / h * (- f_to_integrate(a + 2 * h) + 4 * f_to_integrate(a + h) - 3 * f_to_integrate(a))          
        #initial_approx_deriv = cos(a)
        constant_initial_correction = 2 * sqrt(t - a) * f_to_integrate(a)#t ** (- q) / gamma(1 - q) * f_to_integrate(a)
        linear_initial_correction =   2 / 3. * sqrt(t - a) * (a + 2 * t) * initial_approx_deriv 
        #linear_initial_correction = t ** (1 - q) / gamma(2 - q) * initial_approx_deriv
        constannt_correction_option = 1
        linear_correction_option = 0
        correction = constannt_correction_option * constant_initial_correction + linear_correction_option * linear_initial_correction
        order = 3
        
        if constannt_correction_option:
            def f(x):
                return f_to_integrate(x) - f_to_integrate(a)
        
        else:
            f = f_to_integrate
        
        if order == 1:
            coeff = h ** (- q)
            values = [gamma(k - q) / gamma(k + 1) * (f(t - k * h) - f(a)) for k in range(N)]
            initial_value_correction = constant_initial_correction
        elif order == 2:
            coeff = h ** (- q) * gamma(- q)
            #values = [(- 1) ** k * gamma(q + 1) / (gamma(k + 1) * gamma(q - k + 1)) * (f(t - (k  - 0.5 * q) * h) - f(a) - linear_correction_option * (t - (k  - 0.5 * q) * h) * initial_approx_deriv) for k in range(N)]
            values = [(- 1) ** k * gamma(q + 1) / (gamma(k + 1) * gamma(q - k + 1)) * (f(t - (k  - 0.5 * q) * h) - f(a) - linear_correction_option * (t - (k  - 0.5 * q) * h) * initial_approx_deriv) for k in range(N)]            
            initial_value_correction = correction
        elif order == 3:
            sqrt_var = sqrt(6)
            b0 = 0.5
            b1 = 0.5
            p0 = (- 3 + sqrt_var) / 12
            p1 = (- 3 - sqrt_var) / 12
            return b0 * A(q, p0, f, t, times) + b1 * A(q, p1, f, t, times) + correction
        else:
            sqrt_var = sqrt(34)
            b0 = 8. / 15
            b1 = (119 - 27 * sqrt_var) / 510
            b2 = (119 + 27 * sqrt_var) / 510
            p0 = 0.
            p1 = (- 3 + sqrt_var) / 20
            p2 = (- 3 - sqrt_var) / 20
            #s0 = 0; s1 = 0; s2 = 0; s3 = 0
            #f0, f1, f2, f3 = OmegaTaylorTerms(q, b0, p0)
            #s0 += f0
            #s1 += f1
            #s2 += f2
            #s3 += f3
            #f0, f1, f2, f3 = OmegaTaylorTerms(q, b1, p1)
            #s0 += f0
            #s1 += f1
            #s2 += f2
            #s3 += f3
            #f0, f1, f2, f3 = OmegaTaylorTerms(q, b2, p2)            
            #s0 += f0
            #s1 += f1
            #s2 += f2
            #s3 += f3

            #print("first", s0)
            #print("second", s1)
            #print("third", s2)
            #print("fourth", s3)
            return b0 * A(q, p0, f, t, times) + b1 * A(q, p1, f, t, times) + b2 * A(q, p2, f, t, times) + correction

        return coeff * sum(values) + initial_value_correction

# -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
# Bombardelli ENDS

# *****************************************************************************************************************************************************************************************
# CONVERGENCE ACCELERATION
# *****************************************************************************************************************************************************************************************

def SubstituteRichardsons(approx_successive_values, k, order, level = - 1):
    with precision(200):
        one = BigFloat(1)
        n = len(approx_successive_values)
        if level > n or level < 0:
            max_n = n
        else:
            max_n = level
        
        richardsons = [value for value in approx_successive_values]
        
        while max_n:
            max_n -= 1
            n -= 1
            new_richardsons = []
            
            for i in range(n):
                new_richardsons.append((k ** order * richardsons[i + 1] - richardsons[i]) / (one * (k ** order - 1)))
                
            richardsons = [value for value in new_richardsons]        
            
            for i in range(n):
                approx_successive_values[- i - 1] = richardsons[- i - 1]
            order += 1
            
def SubstituteEmpiricalRichardsons(approx_successive_values, k, order, level = - 1):
    with precision(200):
        one = BigFloat(1)
        n = len(approx_successive_values)
        if level > n or level < 0:
            max_n = n
        else:
            max_n = level
        
        richardsons = [value for value in approx_successive_values]
        while max_n:
            max_n -= 1
            n -= 1
            new_richardsons = []

            for i in range(n):
                approx_order = order
                
                if i > 0:
                    eiminus1 = abs((richardsons[i] - richardsons[i - 1]) / (one * richardsons[i]))
                    ei = abs((richardsons[i + 1] - richardsons[i]) / (one * richardsons[i + 1]))
                    approx_order = max(log(eiminus1 / (one * ei)) / log(one * k), order)
                
                new_richardsons.append((k ** approx_order * richardsons[i + 1] - richardsons[i]) / (one * (k ** approx_order - 1)))

            richardsons = [value for value in new_richardsons]        
            
            for i in range(n):
                approx_successive_values[- i - 1] = richardsons[- i - 1]
            order += 1            

def SubstituteShanks(approx_sequence):
    with precision(200):
        one = BigFloat(1)
        my_list = approx_sequence
        shanks = [(my_list[i + 1] * my_list[i - 1] - my_list[i] ** 2) / (one * (my_list[i + 1] - 2 * my_list[i] + my_list[i - 1])) for i in range(1, len(my_list) - 1)]
        
        while len(shanks) > 2:        
            for i in range(len(shanks)):
                my_list[- i - 1] = shanks[- i - 1]
            temp_shanks = [(shanks[i + 1] * shanks[i - 1] - shanks[i] ** 2) / (one * (shanks[i + 1] - 2 * shanks[i] + shanks[i - 1])) for i in range(1, len(shanks) - 1)]
            shanks = temp_shanks

#****************************************************************************************************************************************************************************************
# MAIN
#****************************************************************************************************************************************************************************************

# Paramters ----------------------------

final_time = 1.0
n_discretizations = 6
min_exp = 2
k = 2
m = 10
order_bomb = 4
f = math.sin
n_div = [k ** (min_exp + i) for i in range(n_discretizations)]
print("Sizes: ", n_div)

# Containers ----------------------------

exact_values = []
approx_values_naive = []
approx_values_1 = []
approx_values_2 = []
approx_values_3 = []
approx_values_bomb = []
approx_values_hins = []
errors_naive = []
errors_1 = []
errors_2 = []
errors_3 = []
errors_bomb = []
errors_hins = []

# Evaluations ----------------------------

for n_divisions in n_div:
    h = final_time / n_divisions 
    times = [h * delta for delta in range(n_divisions)]
    times.append(final_time)
    exact_value = float(ExactIntegrationOfSinus(times[-1]))
    exact_values.append(exact_value)
    approx_value_naive = ApproximateQuadrature(times, f)
    approx_value_1 = Daitche(times, f, 1)
    approx_value_2 = Daitche(times, f, 2)
    approx_value_3 = Daitche(times, f, 3)
    approx_value_bomb = Bombardelli(times, f, order_bomb)
    approx_value_hins = Hinsberg(m, 0.01 * final_time, times, f)    
    approx_values_naive.append(approx_value_naive)
    approx_values_1.append(approx_value_1)
    approx_values_2.append(approx_value_2)
    approx_values_3.append(approx_value_3)
    approx_values_bomb.append(approx_value_bomb)
    approx_values_hins.append(approx_value_hins)    
    errors_naive.append(abs((approx_value_naive - exact_value) / exact_value))
    errors_1.append(abs((approx_value_1 - exact_value) / exact_value))
    errors_2.append(abs((approx_value_2 - exact_value) / exact_value))
    errors_3.append(abs((approx_value_3 - exact_value) / exact_value))
    errors_bomb.append(abs((approx_value_bomb - exact_value) / exact_value))
    errors_hins.append(abs((approx_value_hins - exact_value) / exact_value))    

# Convergence acceleration ----------------------------

approx_values_naive_rich = [value for value in approx_values_naive] 
approx_values_1_rich = [value for value in approx_values_1] 
approx_values_2_rich = [value for value in approx_values_2] 
approx_values_3_rich = [value for value in approx_values_3]
approx_values_bomb_rich = [value for value in approx_values_bomb]
approx_values_hins_rich = [value for value in approx_values_hins]
approx_values_naive_rich_emp = [value for value in approx_values_naive] 
approx_values_1_rich_emp = [value for value in approx_values_1] 
approx_values_2_rich_emp = [value for value in approx_values_2] 
approx_values_3_rich_emp = [value for value in approx_values_3]
approx_values_bomb_rich_emp = [value for value in approx_values_bomb]
approx_values_hins_rich_emp = [value for value in approx_values_hins] 

#approx_values_naive_rich[-1] = mpmath.richardson(approx_values_naive_rich)[0]
#approx_values_1_rich[-1] = mpmath.richardson(approx_values_1_rich)[0]
#approx_values_2_rich[-1] = mpmath.richardson(approx_values_2_rich)[0]
#approx_values_3_rich[-1] = mpmath.richardson(approx_values_3_rich)[0]
SubstituteRichardsons(approx_values_naive_rich, k, 0.5)
SubstituteRichardsons(approx_values_1_rich, k, 2)
SubstituteRichardsons(approx_values_2_rich, k, 3)
SubstituteRichardsons(approx_values_3_rich, k, 4)
SubstituteRichardsons(approx_values_bomb_rich, k, order_bomb)
SubstituteRichardsons(approx_values_hins_rich, k, 1)
SubstituteEmpiricalRichardsons(approx_values_naive_rich_emp, k, 0.5)
SubstituteEmpiricalRichardsons(approx_values_1_rich_emp, k, 2)
SubstituteEmpiricalRichardsons(approx_values_2_rich_emp, k, 3)
SubstituteEmpiricalRichardsons(approx_values_3_rich_emp, k, 4)
SubstituteEmpiricalRichardsons(approx_values_bomb_rich_emp, k, 2)
SubstituteEmpiricalRichardsons(approx_values_hins_rich_emp, k, 1)
approx_values_naive_shank = [value for value in approx_values_naive]
approx_values_1_shank = [value for value in approx_values_1] 
approx_values_2_shank = [value for value in approx_values_2] 
approx_values_3_shank = [value for value in approx_values_3]
approx_values_bomb_shank = [value for value in approx_values_bomb]
approx_values_hins_shank = [value for value in approx_values_hins] 
SubstituteShanks(approx_values_naive_shank)
SubstituteShanks(approx_values_1_shank)
SubstituteShanks(approx_values_2_shank)
SubstituteShanks(approx_values_3_shank)
SubstituteShanks(approx_values_bomb_shank)
SubstituteShanks(approx_values_hins_shank)

# Computing errors ----------------------------

errors_naive_rich = [abs((approx_values_naive_rich[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_1_rich = [abs((approx_values_1_rich[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_2_rich = [abs((approx_values_2_rich[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_3_rich = [abs((approx_values_3_rich[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_bomb_rich = [abs((approx_values_bomb_rich[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_hins_rich = [abs((approx_values_hins_rich[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_naive_rich_emp = [abs((approx_values_naive_rich_emp[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_1_rich_emp = [abs((approx_values_1_rich_emp[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_2_rich_emp = [abs((approx_values_2_rich_emp[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_3_rich_emp = [abs((approx_values_3_rich_emp[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_bomb_rich_emp = [abs((approx_values_bomb_rich_emp[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_hins_rich_emp = [abs((approx_values_hins_rich_emp[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_naive_shank = [abs((approx_values_naive_shank[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_1_shank = [abs((approx_values_1_shank[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_2_shank = [abs((approx_values_2_shank[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_3_shank = [abs((approx_values_3_shank[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_bomb_shank = [abs((approx_values_bomb_shank[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
errors_hins_shank = [abs((approx_values_hins_shank[i] - exact_values[i]) / exact_values[i]) for i in range(len(exact_values))]
theoretical_slope_naive = []
theoretical_slope_naive = [errors_naive[0] * 0.5 ** (i / 2)          for i in range(len(n_div))]
theoretical_slope_1 =     [errors_1[0]     * 0.5 ** (2 * i)          for i in range(len(n_div))]
theoretical_slope_2 =     [errors_2[0]     * 0.5 ** (3 * i)          for i in range(len(n_div))]
theoretical_slope_3 =     [errors_3[0]     * 0.5 ** (4 * i)          for i in range(len(n_div))]
theoretical_slope_bomb =  [errors_bomb[0]  * 0.5 ** (order_bomb * i) for i in range(len(n_div))]
theoretical_slope_hins =  [errors_hins[0]  * 0.5 ** (2 * i)          for i in range(len(n_div))]

# Plotting ----------------------------

plt.plot(n_div, errors_naive, color='r')
plt.plot(n_div, errors_1, color='b')
plt.plot(n_div, errors_2, color='g')
plt.plot(n_div, errors_3, color='k')
plt.plot(n_div, errors_bomb, color='c')
plt.plot(n_div, errors_hins, color='m')
plt.plot(n_div, errors_naive_rich, color='r', linestyle = '--')
plt.plot(n_div, errors_1_rich, color='b', linestyle = '--')
plt.plot(n_div, errors_2_rich, color='g', linestyle = '--')
plt.plot(n_div, errors_3_rich, color='k', linestyle = '--')
plt.plot(n_div, errors_bomb_rich, color='c', linestyle = '--')
plt.plot(n_div, errors_hins_rich, color='m', linestyle = '--')
#plt.plot(n_div, errors_naive_rich_emp, color='r', linestyle = '-.')
#plt.plot(n_div, errors_1_rich_emp, color='b', linestyle = '-.')
#plt.plot(n_div, errors_2_rich_emp, color='g', linestyle = '-.')
#plt.plot(n_div, errors_3_rich_emp, color='k', linestyle = '-.')
plt.plot(n_div, errors_bomb_rich_emp, color='c', linestyle = '-.')
#plt.plot(n_div, errors_hins_rich_emp, color='m', linestyle = '-.')
#plt.plot(n_div, errors_naive_shank, color='r', linestyle = '-.')
#plt.plot(n_div, errors_1_shank, color='b', linestyle = '-.')
#plt.plot(n_div, errors_2_shank, color='g', linestyle = '-.')
#plt.plot(n_div, errors_3_shank, color='k', linestyle = '-.')
#plt.plot(n_div, errors_bomb_shank, color='c', linestyle = '-.')
plt.plot(n_div, errors_hins_shank, color='m', linestyle = '-.')
plt.plot(n_div, theoretical_slope_naive, color='r', linestyle = ':')
plt.plot(n_div, theoretical_slope_1, color='b', linestyle = ':')
plt.plot(n_div, theoretical_slope_2, color='g', linestyle = ':')
plt.plot(n_div, theoretical_slope_3, color='k', linestyle = ':')
plt.plot(n_div, theoretical_slope_bomb, color='c', linestyle = ':')
plt.plot(n_div, theoretical_slope_hins, color='m', linestyle = ':')
plt.loglog()
plt.show()