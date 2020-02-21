
/**
 * Reads the log pins and print there value in the monitor.
 * 1 => false
 * 0 => true
 */
void dumpStatus() {
  Serial.print("OUT_OK_PIN: ");
  Serial.print(digitalRead(OUT_OK_PIN));
  Serial.print(" LOWER_PIN: ");
  Serial.print(digitalRead(LOWER_PIN));
  Serial.print(" ERROR_PIN: ");
  Serial.print(digitalRead(ERROR_PIN));
  Serial.print(" EMPTY_PIN: ");
  Serial.print(digitalRead(EMPTY_PIN));
  Serial.print("\n");
}

/**
 * 
 */
void dispendCard() {
  digitalWrite(VD_PIN, LOW);
  digitalWrite(PAYOUT_PIN, LOW);
  
  // Send reset
  digitalWrite(RESET_PIN, HIGH);
  delay(300);
  digitalWrite(RESET_PIN, LOW);
  delay(300);
  digitalWrite(VD_PIN, HIGH);
  digitalWrite(PAYOUT_PIN, HIGH);
  delay(300);
}
