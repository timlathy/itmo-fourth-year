package itmo.se.lab3

import itmo.se.lab3.helpers.*
import itmo.se.lab3.pages.HomePage
import itmo.se.lab3.pages.JobSearchPage
import org.junit.jupiter.api.Assertions.*
import org.junit.jupiter.api.TestTemplate
import org.junit.jupiter.api.extension.ExtendWith
import org.openqa.selenium.WebDriver

@ExtendWith(WebDriverTemplateInvocationContextProvider::class)
class JobSearchTest {
    @TestTemplate
    @RunWithChrome
    @RunWithFirefox
    fun `it allows users to sort jobs by price`(driver: WebDriver) {
        val homePage = HomePage(driver)
        homePage.logIn(UserCredentials.email, UserCredentials.password)

        val jobPage = JobSearchPage(driver)
        val jobs = jobPage.listItems()

        jobPage.sortSelector.selectByVisibleText("по цене")
        jobPage.waitForItems()
        val jobsByPrice = jobPage.listItems()

        assertTrue(jobsByPrice[0].price > jobs[0].price)
    }

    @TestTemplate
    @RunWithChrome
    @RunWithFirefox
    fun `it allows users to filter jobs by category`(driver: WebDriver) {
        val homePage = HomePage(driver)
        homePage.logIn(UserCredentials.email, UserCredentials.password)

        val jobPage = JobSearchPage(driver)
        val jobs = jobPage.listItems()
        assertNotEquals("TEXT / Новая тема в форуме", jobs[0].getCategory())

        val categoryCheckbox = jobPage.getCategories().find { it.text == "TEXT / Новая тема в форуме" }!!
        assertFalse(categoryCheckbox.checked)
        categoryCheckbox.toggle()
        jobPage.searchButton.click()
        jobPage.waitForItems()

        val jobsByCategory = jobPage.listItems()
        for (job in jobsByCategory)
            assertEquals("TEXT / Новая тема в форуме", job.getCategory())
    }
}