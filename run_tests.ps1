# VPN Automated Test Suite
# Run with: powershell -ExecutionPolicy Bypass -File run_tests.ps1

param(
    [string]$ServerIP = "10.0.0.1",
    [string]$OutputFile = "test_results.txt"
)

$ErrorActionPreference = "Continue"
$timestamp = Get-Date -Format "yyyy-MM-dd HH:mm:ss"

# Initialize output
"=" * 80 | Out-File -FilePath $OutputFile
"VPN Test Suite Results" | Out-File -FilePath $OutputFile -Append
"Timestamp: $timestamp" | Out-File -FilePath $OutputFile -Append
"=" * 80 | Out-File -FilePath $OutputFile -Append

Write-Host "=====================================" -ForegroundColor Green
Write-Host "     VPN Automated Test Suite        " -ForegroundColor Green
Write-Host "=====================================" -ForegroundColor Green
Write-Host ""

# Test 1: Connectivity Test
Write-Host "[Test 1/6] Connectivity Test (Ping)" -ForegroundColor Cyan
"`n[Test 1] Connectivity Test (Ping)" | Out-File -FilePath $OutputFile -Append
try {
    $pingResult = ping $ServerIP -n 10
    $pingResult | Out-File -FilePath $OutputFile -Append
    
    if ($pingResult -match "Reply from") {
        Write-Host "  ✅ PASS - Server is reachable" -ForegroundColor Green
        "  Result: PASS" | Out-File -FilePath $OutputFile -Append
    } else {
        Write-Host "  ❌ FAIL - No response from server" -ForegroundColor Red
        "  Result: FAIL" | Out-File -FilePath $OutputFile -Append
    }
} catch {
    Write-Host "  ❌ ERROR - $_" -ForegroundColor Red
    "  Result: ERROR - $_" | Out-File -FilePath $OutputFile -Append
}

# Test 2: Extended Ping (100 packets)
Write-Host "`n[Test 2/6] Extended Connectivity (100 packets)" -ForegroundColor Cyan
"`n[Test 2] Extended Connectivity Test" | Out-File -FilePath $OutputFile -Append
try {
    $extPingResult = ping $ServerIP -n 100
    $extPingResult | Out-File -FilePath $OutputFile -Append
    
    # Parse results
    if ($extPingResult -match "(\d+)% loss") {
        $loss = $matches[1]
        if ([int]$loss -eq 0) {
            Write-Host "  ✅ PASS - 0% packet loss" -ForegroundColor Green
        } else {
            Write-Host "  ⚠️  WARN - ${loss}% packet loss" -ForegroundColor Yellow
        }
        "  Packet Loss: ${loss}%" | Out-File -FilePath $OutputFile -Append
    }
} catch {
    Write-Host "  ❌ ERROR - $_" -ForegroundColor Red
}

# Test 3: Traceroute
Write-Host "`n[Test 3/6] Route Tracing" -ForegroundColor Cyan
"`n[Test 3] Route Tracing" | Out-File -FilePath $OutputFile -Append
try {
    $traceResult = tracert -d -h 5 $ServerIP
    $traceResult | Out-File -FilePath $OutputFile -Append
    Write-Host "  ✅ Traceroute completed" -ForegroundColor Green
} catch {
    Write-Host "  ❌ ERROR - $_" -ForegroundColor Red
}

# Test 4: Different Packet Sizes
Write-Host "`n[Test 4/6] Variable Packet Sizes" -ForegroundColor Cyan
"`n[Test 4] Variable Packet Sizes" | Out-File -FilePath $OutputFile -Append

$packetSizes = @(64, 512, 1400, 1472)
foreach ($size in $packetSizes) {
    Write-Host "  Testing ${size} bytes..." -NoNewline
    "  Packet Size: ${size} bytes" | Out-File -FilePath $OutputFile -Append
    
    $sizeTest = ping $ServerIP -n 5 -l $size
    $sizeTest | Out-File -FilePath $OutputFile -Append
    
    if ($sizeTest -match "Reply from") {
        Write-Host " ✅" -ForegroundColor Green
    } else {
        Write-Host " ❌" -ForegroundColor Red
    }
}

# Test 5: Interface Statistics
Write-Host "`n[Test 5/6] Network Interface Statistics" -ForegroundColor Cyan
"`n[Test 5] Network Interface Statistics" | Out-File -FilePath $OutputFile -Append
try {
    $interfaces = Get-NetAdapter | Where-Object {$_.Name -like "*VPN*"}
    $interfaces | Format-Table Name, Status, LinkSpeed, MacAddress | Out-File -FilePath $OutputFile -Append
    $interfaces | Format-Table Name, Status, LinkSpeed | Out-Host
    Write-Host "  ✅ Interface info retrieved" -ForegroundColor Green
} catch {
    Write-Host "  ⚠️  No VPN interfaces found" -ForegroundColor Yellow
}

# Test 6: DNS Resolution (if applicable)
Write-Host "`n[Test 6/6] Route Table Check" -ForegroundColor Cyan
"`n[Test 6] Route Table" | Out-File -FilePath $OutputFile -Append
try {
    $routes = route print | Select-String "10.0.0"
    $routes | Out-File -FilePath $OutputFile -Append
    if ($routes) {
        Write-Host "  ✅ VPN routes found" -ForegroundColor Green
        $routes | Out-Host
    } else {
        Write-Host "  ⚠️  No VPN routes detected" -ForegroundColor Yellow
    }
} catch {
    Write-Host "  ❌ ERROR - $_" -ForegroundColor Red
}

# Summary
Write-Host "`n=====================================" -ForegroundColor Green
Write-Host "     Test Suite Completed            " -ForegroundColor Green
Write-Host "=====================================" -ForegroundColor Green
Write-Host "Results saved to: $OutputFile" -ForegroundColor Cyan
Write-Host ""

"`n" + "=" * 80 | Out-File -FilePath $OutputFile -Append
"Test Suite Completed at $(Get-Date -Format 'yyyy-MM-dd HH:mm:ss')" | Out-File -FilePath $OutputFile -Append
"=" * 80 | Out-File -FilePath $OutputFile -Append
