#include <cassert>
#include <cctype>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct BigInteger {
  /* For stream input */
  friend istream& operator>>(istream& is, BigInteger& rhs);

  /* For stream output */
  friend ostream& operator<<(ostream& os, const BigInteger& rhs);

 public:
  /* Default Constructor
   * You will get a number with zero value.
   */
  BigInteger();

  /* Constructor
   * If this number is positive, then sign is true, otherwise false.
   */
  explicit BigInteger(bool sign, const string& str);

  /* Constructor */
  explicit BigInteger(const string& str);

  /* Constructor */
  explicit BigInteger(long long num);

  /* Copy Constructor */
  BigInteger(const BigInteger& rhs);

  /* Copy Assignement Operator */
  BigInteger& operator=(const BigInteger& rhs);

  /* Operator Add */
  BigInteger operator+(const BigInteger& rhs);

  /* Operator Multiply */
  BigInteger operator*(const BigInteger& rhs);

 private:
  bool isdigit(const char c) { return c >= '0' && c <= '9'; }

  /* Check if all character in the range from pos
   * to pos + size - 1 on @str is digit. */
  bool isdigit(const string& str, int pos, int size) {
    for (int i = 0; i < size; i++)
      if (!isdigit(str[pos + i])) return false;
    return true;
  }

  BigInteger(bool sign, const vector<int>& s);

  /* return -1, if a <  b
   * return  0, if a == b
   * return  1, if a >  b
   */
  bool cmp(const vector<int>& a, const vector<int>& b) {
    if (a.size() < b.size()) return -1;
    if (a.size() > b.size()) return 1;

    int n = a.size();
    for (int i = n - 1; i >= 0; i--) {
      if (a[i] < b[i]) return -1;
      if (a[i] > b[i]) return 1;
    }
    return 0;
  }

  /* The maximum number of each digit = BASE - 1 */
  static const int BASE = 100000000;

  /* The width of BASE */
  static const int WIDTH = 8;

  /* If this is a positive number, then sign is true,
   * otherwise false.
   */
  bool sign;

  /* Vector s stores all digits from
   * Least Significant Digit to Most Siginificant Digit.
   */
  vector<int> s;
};

istream& operator>>(istream& is, BigInteger& rhs) {
  string str;
  is >> str;
  rhs = BigInteger(str);
  return is;
}

ostream& operator<<(ostream& os, const BigInteger& rhs) {
  const vector<int>& s = rhs.s;
  // Special Case: zero.
  if (s.size() == 1 && s[0] == 0) {
    os << "0";
    return os;
  }

  assert(s.size() >= 1);
  if (rhs.sign == false) os << "-";
  os << to_string(s.back());

  for (int i = (int)s.size() - 2; i >= 0; i--) {
    os << setw(BigInteger::WIDTH) << setfill('0') << to_string(s[i]);
  }
  return os;
}

/* Definition of Default Constructor
 * Vector s has only one value, zero.
 */
BigInteger::BigInteger() : sign(true), s(1, 0) {}

BigInteger::BigInteger(bool sign, const string& str) : sign(sign) {
  // Assume all characters in @str is digit.

  int start_pos = 0;
  // Skip all leading zero.
  while (str[start_pos] == '0') start_pos++;
  int r = str.size(), p = max(start_pos, r - WIDTH);
  while (r > start_pos) {
    const string sstr = str.substr(p, r - p);
    s.push_back(stoi(sstr));
    r -= WIDTH;
    p = max(start_pos, r - WIDTH);
  }
}

BigInteger::BigInteger(const string& str) {
  // Assume str is not empty.
  assert(str.empty() == false);

  // If @str has a sign symbol, then it must in the front.
  if (str[0] == '-')
    sign = false;
  else
    sign = true;

  // Assume All other characters are digit.

  // the start index of digit.
  int start_pos = (isdigit(str[0]) ? 0 : 1);
  // Skip all leading zero.
  while (str[start_pos] == '0') start_pos++;

  int r = str.size(), p = max(start_pos, r - WIDTH);
  while (r > start_pos) {
    const string sstr = str.substr(p, r - p);
    s.push_back(stoi(sstr));
    r -= WIDTH;
    p = max(start_pos, r - WIDTH);
  }
}

BigInteger::BigInteger(long long num) {
  if (num < 0) num = -num;
  sign = num >= 0;
  if (num == 0) {
    s.push_back(0);
    return;
  }
  while (num) {
    s.push_back(num % BASE);
    num /= BASE;
  }
}

BigInteger::BigInteger(const BigInteger& rhs) : sign(rhs.sign), s(rhs.s) {}

BigInteger::BigInteger(bool sign, const vector<int>& s) : sign(sign), s(s) {}

BigInteger& BigInteger::operator=(const BigInteger& rhs) {
  sign = rhs.sign;
  s = rhs.s;
  return *this;
}

BigInteger BigInteger::operator+(const BigInteger& rhs) {
  bool new_sign;
  vector<int> new_vec;

  const vector<int>& a = s;
  const vector<int>& b = rhs.s;
  if (sign == rhs.sign) {
    new_sign = sign;
    int i = 0, j = 0, carry = 0;
    while (i < a.size() || j < b.size() || carry) {
      int x = i < a.size() ? a[i] : 0;
      int y = j < b.size() ? b[i] : 0;
      int sum = x + y + carry;
      new_vec.push_back(sum % BASE);
      carry /= BASE;
      i++;
      j++;
    }
    return BigInteger(new_sign, new_vec);
  }

  int cmp_res = cmp(a, b);
  if (cmp_res == 0) return BigInteger();  // a == b

  // a < b
  if (cmp_res > 0) {
    new_sign = rhs.sign;
    int i = 0, j = 0, carry = 0;
    while (i < a.size() || j < b.size() || carry) {
      int x = i < a.size() ? a[i] : 0;
      int y = j < b.size() ? b[j] : 0;
      int sub = y - x + carry;
      if (sub < 0) {
        sub += BASE;
        carry = -1;
      } else {
        carry = 0;
      }
      new_vec.push_back(sub);
      i++;
      j++;
    }
    while (new_vec.back() == 0) new_vec.pop_back();
  } else {
    // a > b
    new_sign = sign;
    int i = 0, j = 0, carry = 0;
    while (i < a.size() || j < b.size() || carry) {
      int x = i < a.size() ? a[i] : 0;
      int y = j < b.size() ? b[j] : 0;
      int sub = x - y + carry;
      if (sub < 0) {
        sub += BASE;
        carry = -1;
      } else {
        carry = 0;
      }
      new_vec.push_back(sub);
      i++;
      j++;
    }
    while (new_vec.back() == 0) new_vec.pop_back();
  }
  return BigInteger(new_sign, new_vec);
}

BigInteger BigInteger::operator*(const BigInteger& rhs) {
  bool new_sign;
  vector<int> new_vec;

  new_sign = (sign == rhs.sign);

  const vector<int>& a = s;
  const vector<int>& b = rhs.s;
  
  new_vec.resize(a.size()+b.size(), 0);
  for (int i = 0; i < a.size(); i++) {
    for (int j = 0; j < b.size(); j++) {
      long long product = (long long) a[i] * b[j];
      
      new_vec[i + j] += product % BASE;

      long long carry = new_vec[i + j] / BASE + product / BASE; // Carry
      new_vec[i + j] %= BASE;                                   // Catch Lowest WIDTH Bit.
      for (int k = 1; carry; k++) {
        new_vec[i + j + k] += carry;
        new_vec[i + j + k] %= BASE;
        carry /= BASE;
      }

    }
  }
  while (new_vec.back() == 0) new_vec.pop_back();
  return BigInteger(new_sign, new_vec);
}

int main() {

  return 0;
}