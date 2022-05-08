#ifndef COMPLEX_H
#define COMPLEX_H

template <typename Tp>
class complex
{
public:
    using value_type = Tp;

    constexpr 
    complex(const Tp& r = Tp(), const Tp& i = Tp()) 
            :m_real(r), m_imag(i) {}
    
    constexpr
    complex(const complex& rhs) 
            :m_real(rhs.m_real), m_imag(rhs.m_imag) {} 

    template <class U>
    constexpr complex(const complex<U>& rhs)
        : m_real(static_cast<Tp>(rhs.real())), m_imag(static_cast<Tp>(rhs.imag())) {} 
    
        


public:
    constexpr Tp real() const { return m_real; }
    constexpr Tp imag() const { return m_imag; }

    constexpr void real(const Tp &r) { m_real = r; }
    constexpr void imag(const Tp &i) { m_imag = i; }

private:
    Tp m_real;
    Tp m_imag;

};


#endif