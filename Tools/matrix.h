#ifndef MATRIX_H
#define MATRIX_H
/*********************************************************************************************************/
template<int N, int M>
class matrix{
public:
    matrix() ;
    matrix( const matrix & t) ;
    float& operator()(int i, int j) {return m[i][j];}
    matrix<N,M>& operator=(const matrix<N,M> & t);
    matrix<N,M> operator+(const matrix<N,M>& B);
    matrix<N,M> operator*(float mult);
    template<int M2>
    matrix<N,M2> operator*(const matrix<M,M2>& B) ;
    float m[N][M];
};

template<int N, int M>
matrix<N,M>::matrix() {
    for(int i=0;i<N;i++)
        for(int j=0;j<M;j++)
            m[i][j]=0.0f;
}
template<int N, int M>
matrix<N,M>::matrix( const matrix & t) {
    for(int i=0;i<N;i++)
        for(int j=0;j<M;j++)
            this->m[i][j] = t.m[i][j];
}
template<int N, int M>
matrix<N,M>& matrix<N,M>::operator=(const matrix<N,M>& to_copy) // copy assignment
{
    for(int i=0;i<N;i++)
        for(int j=0;j<M;j++)
            m[i][j] = to_copy.m[i][j];
    return *this;
}
template<int N, int M>
matrix<N,M> matrix<N,M>::operator+(const matrix<N,M>& B) // A+B
{
    matrix<N,M> result;
    for(int i=0;i<N;i++)
        for(int j=0;j<M;j++)
            result.m[i][j] = B.m[i][j] + this->m[i][j];
    return result;
}
template<int N, int M>
matrix<N,M> matrix<N,M>::operator*(float mult) // A*alpha
{
    matrix<N,M> result;
    for(int i=0;i<N;i++)
        for(int j=0;j<M;j++)
            result.m[i][j] = m[i][j] * mult;
    return result;
}
template<int N, int M>
template<int M2>
matrix<N,M2> matrix<N,M>::operator*(const matrix<M,M2>& B)
{
    matrix<N,M2> result;
    for(int r = 0; r < N; r++)
    {
        for(int c = 0; c < M2; c++)
        {
            for( int i=0; i < M; i++ ){
               result(r,c) = result(r,c) + this->m[r][i] * B.m[i][c];
            }
        }
    }
    return result;
}
/*********************************************************************************************************/
#endif // MATRIX_H
