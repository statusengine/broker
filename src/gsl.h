//
// gsl-lite is based on GSL: Guidelines Support Library.
// For more information see https://github.com/gsl-lite/gsl-lite
//
// Copyright (c) 2015-2018 Martin Moene
// Copyright (c) 2015-2018 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#pragma once

namespace gsl {

template< class F >
class final_action
{
public:
    explicit final_action( F action ) noexcept
        : action_( std::move( action ) )
        , invoke_( true )
    {}

    final_action( final_action && other ) noexcept
        : action_( std::move( other.action_ ) )
        , invoke_( other.invoke_ )
    {
        other.invoke_ = false;
    }

    virtual ~final_action() noexcept
    {
        if ( invoke_ )
            action_();
    }

    final_action( final_action const  & ) = delete;
    final_action & operator=( final_action const & ) = delete;
    final_action & operator=( final_action && ) = delete;

protected:
    void dismiss() noexcept
    {
        invoke_ = false;
    }

private:
    F action_;
    bool invoke_;
};

template< class F >
inline final_action<F> finally( F const & action ) noexcept
{
    return final_action<F>( action );
}

template< class F >
inline final_action<F> finally( F && action ) noexcept
{
    return final_action<F>( std::forward<F>( action ) );
}

}

